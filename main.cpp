#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#define _USE_MATH_DEFINES
#include <cmath>
#include "utils.h"
#include SDL2_H
#include SDL2_TTF_H
#include "rays.h"
#include "render.h"
#include "gui.h"
namespace fs = std::filesystem;

#define QUITGAME quit(textures, floortex, ceiltex, ch, window, surface)
#define ERROR_RETURN(code) \
	logfile << SDL_GetError() << std::endl; QUITGAME; \
	logfile.close(); \
	return code;

#define MAPVER 1
Uint16 tiles[MAPW][MAPH];
SDL_Surface* textures[15];
SDL_Surface* floortex;
SDL_Surface* ceiltex;
SDL_Surface* ch; // crosshair
std::ofstream logfile("logfile.txt", std::ofstream::out);
float fov = (M_PI / 2) / 2;

static int SDLCALL filter(void* userdata, SDL_Event* e)
{
	return e->type == SDL_QUIT ||
	       e->type == SDL_TEXTINPUT ||
	       (e->type == SDL_KEYDOWN && (userdata || !e->key.repeat)) ||
	       e->type == SDL_MOUSEBUTTONDOWN;
}

int loadmap(std::string name, bool reset_on_fail = false)
{
	int e = 0, v;
	name = "maps/" + name;
	std::ifstream file(name, std::ios::binary);
	file >> v;
	if (v != MAPVER && ++e)
		logfile << "Invalid map version. Expected " << MAPVER << ", but got " << v;
	else
		file.ignore().read((char*) tiles, MAPH * MAPW * 2);
	if (!file.good() && ++e)
	{
		logfile << "Error while loading '" << name << "'" << std::endl;
		// set to default map
		if (reset_on_fail)
			for (int y = 0; y < MAPH; y++) for (int x = 0; x < MAPW; x++)
				tiles[y][x] = !(x % (MAPW - 1) && y % (MAPH - 1)) * 0x1111;
	}
	file.close();
	return e;
}

int savemap(std::string name)
{
	int e = 0;
	name = "maps/" + name;
	std::ofstream file(name, std::ios::binary);
	file << MAPVER << std::endl;
	file.write((char*) tiles, MAPH * MAPW * 2);
	if (!file.good() && ++e) logfile << "Error while saving '" << name << "'" << std::endl;
	file.close();
	return e;
}

enum ThingAlignment {
	/************************************************/ ALIGN_TOP = 0,
	/************* Reference Rectangle **************/ ALIGN_MIDDLE = 1,
	/************************************************/ ALIGN_BOTTOM = 2,
	ALIGN_LEFT = 3, ALIGN_CENTER = 4, ALIGN_RIGHT = 5,
};
// returns a pointer to the GUIThing that was added
GUIThing* addthing(
	std::vector<GUIThing>& guithings,
	GUIThing thing,
	ThingAlignment align = ALIGN_CENTER,
	SDL_Rect *ref = NULL,
	int margin = 5
)
{
	if (!ref && !guithings.size()) { guithings.push_back(thing); return &guithings.back(); }
	if (!ref) ref = &guithings.back().r;
	if (align < 3)
	{
		thing.r.x = ref->x + ref->w + margin;
		thing.r.y = ref->y + align * (ref->h - thing.r.h) / 2;
	}
	else
	{
		thing.r.y = ref->y + ref->h + margin;
		thing.r.x = ref->x + (align - 3) * (ref->w - thing.r.w) / 2;
	}
	guithings.push_back(thing);
	return &guithings.back();
}

GUIThing* startinput(TTF_Font* font, GUIThing* box)
{
	SDL_StartTextInput();
	redrawinput(font, box);
	return box;
}
GUIThing* stopinput(TTF_Font* font, GUIThing* focused)
{
	SDL_StopTextInput();
	redrawinput(font, focused, false);
	return NULL;
}

int main(int argc, char* argv[])
{
	// go to the app's directory
	fs::current_path(((fs::path) argv[0]).parent_path());
	// initialize stuff
	SDL_Window*  window;
	SDL_Surface* surface;
	if (init(window, surface) || TTF_Init()) { ERROR_RETURN(1); }
	GUIThing* focused = NULL;
	SDL_SetEventFilter(filter, &focused);
	// load textures and map
	if (loadtex(textures, floortex, ceiltex, ch)) { ERROR_RETURN(2); }
	std::string mapname = "maze";
	loadmap(mapname, true);
	TTF_Font* font = TTF_OpenFont("font.ttf", 14);
	if (font == NULL) { logfile << TTF_GetError() << std::endl; logfile.close(); return 3; }
	char infostr[64]; SDL_Surface* text;
	// set up gui things
	std::vector<GUIThing> guithings;
	// reserve enough memory so that the pointers remain valid
	guithings.reserve(3);
	// add things
	GUIThing *namebox = addthing(guithings, inputbox(font, "Map name:", 100, GRAY(0xff))),
	         *savebut = addthing(guithings, button(font, "Save Map"), ALIGN_LEFT),
	         *loadbut = addthing(guithings, button(font, "Load Map"), ALIGN_RIGHT, &namebox->r);
	GUIThing guibdr = backdrop(guithings, font, mapname.c_str());
	// move gui things to center of screen
	{
		SDL_Point offset = { (WIDTH - guibdr.r.w) / 2, (HEIGHT - guibdr.r.h) / 2 };
		offset.x -= guibdr.r.x; offset.y -= guibdr.r.y;
		guibdr.r.x += offset.x; guibdr.r.y += offset.y;
		for (GUIThing& g : guithings) { g.r.x += offset.x; g.r.y += offset.y; }
	}
	bool showgui = false;
	// set up variables for game
	Vec2d<float> pos = { MAPW / 2.f, MAPH / 2.f, -1};
	float heading = 0;
	// normalized coordinates from leftmost fov to rightmost
	Vec2d<float> fieldleft, fieldcenter = { 1, 0, 1 }, fieldright;
	// editing variables
	bool editmode = false; Vec2d<int> hl;
	// main loop variables
	Uint32 lasttick = SDL_GetTicks(), tdiff = 0;
	const Uint8* kb = SDL_GetKeyboardState(NULL);
	bool running = true;
	// main loop
	while (running)
	{
		// update player
		if (!focused)
		{
			// positive or negative depending on what keys are pressed
			float multiplier = (kb[SDL_SCANCODE_W] - kb[SDL_SCANCODE_S])
			                   * (float) tdiff / 250.f;
			Vec2d<float> last = pos;
			// update position accounting for heading, and then checking collision
			pos.x += multiplier * fieldcenter.x;
			if (tiles[(int) pos.y][(int) pos.x]) pos.x = last.x;
			pos.y += multiplier * fieldcenter.y;
			if (tiles[(int) pos.y][(int) pos.x]) pos.y = last.y;
			// turning
			heading += (kb[SDL_SCANCODE_D] - kb[SDL_SCANCODE_A]) 
			           * (float) tdiff / 400.f;
			// update fov variables
			fieldleft   = { cos(heading - fov), sin(heading - fov), 1 };
			fieldcenter = { cos(heading),       sin(heading),       1 };
			fieldright  = { cos(heading + fov), sin(heading + fov), 1 };
			// highlight the face under the crosshair
			if (editmode) raycast(pos, fieldcenter, tiles, hl);
		}
		// render (floor and ceiling) and walls
		renderfloors(surface, pos, fieldleft, fieldright, floortex, ceiltex);
		renderwalls(
			surface,
			pos, heading, editmode,
			tiles,
			fieldleft, fieldright,
			hl,
			textures
		);
		// render info text
		heading = fmod(heading, 2 * M_PI);
		heading += (heading < 0) * 2 * M_PI;
		sprintf(
			infostr,
			"%i deg | %4.1f fps",
			(int) (heading * 180 / M_PI),
			1000 / (float) tdiff
		);
		text = TTF_RenderText_Solid(font, infostr, (SDL_Color) { 255, 255, 255, 255 });
		SDL_Rect rect = { 0, 0, text->w, text->h };
		SDL_BlitSurface(text, NULL, surface, &rect);
		SDL_FreeSurface(text);
		// crosshair
		rect = { (WIDTH - ch->w) / 2, (HEIGHT - ch->h) / 2, ch->w, ch->h };
		SDL_BlitSurface(ch, NULL, surface, &rect);
		// gui
		if (showgui)
		{
			SDL_BlitSurface(guibdr.s, NULL, surface, &guibdr.r);
			for (GUIThing g : guithings) SDL_BlitSurface(g.s, NULL, surface, &g.r);
		}
		SDL_UpdateWindowSurface(window);

		// process events
		for (SDL_Event e; SDL_PollEvent(&e) && (running = e.type != SDL_QUIT);)
		{
			switch (e.type)
			{
			case SDL_TEXTINPUT:
				if (!focused || focused->overflown) break;
				focused->value += e.text.text;
				redrawinput(font, focused);
				break;
			case SDL_KEYDOWN:
			{
				SDL_Keycode keycode = e.key.keysym.sym;
				if (focused)
				{
					switch (keycode)
					{
					case SDLK_ESCAPE:
					case SDLK_RETURN:
						focused = stopinput(font, focused);
						goto exit_typeswitch;
					case SDLK_BACKSPACE:
						if (!focused->value.size()) break;
						if (!focused->overflown) focused->value.pop_back();
						redrawinput(font, focused);
						break;
					}
					break;
				}
				// texture information for the highlighted tile/face
				Uint16 current = editmode ? tiles[hl.y][hl.x] : 0;
				Uint16 facetex = current >> hl.mag * 4 & 0xf;
				Uint16 oldtex  = facetex;
				switch (keycode)
				{
				case SDLK_e:    editmode = !editmode; break;
				case SDLK_DOWN: facetex--; break;
				case SDLK_UP:   facetex++; break;
				case SDLK_ESCAPE: showgui = !showgui; break;
				}
				// update the texture if needed
				if (editmode && facetex != oldtex)
				{
					facetex = (facetex - 1) % 15 + 1;
					facetex += (facetex <= 0) * 15;
					tiles[hl.y][hl.x] = current & ~(0xf << hl.mag * 4)
							    | facetex << hl.mag * 4;
				}
				break;
			}
			case SDL_MOUSEBUTTONDOWN:
			{
				if (!showgui) break;
				SDL_Point p = { e.button.x, e.button.y };
				bool f = focused;
				if (f) focused = stopinput(font, focused);
				int i = 0, limit = guithings.size();
				for (; i < limit; i++)
				{
					GUIThing* g = &guithings[i];
					if (!SDL_PointInRect(&p, &g->r)) continue;
					if (g->type == GUI_INPUT && !focused)
						focused = startinput(font, g);
					else
					{
						if (g == loadbut && !loadmap(namebox->value))
							guibdr = backdrop(
								guithings, font,
								(mapname = namebox->value).c_str()
							);
						else if (g == savebut)
							savemap(namebox->value);
					}
					break;
				}
				// close gui if click out and no focused input box
				showgui = f || i != limit || SDL_PointInRect(&p, &guibdr.r);
			}
			}
			exit_typeswitch: continue;
		}
		// tick the clock
		tdiff = (SDL_GetTicks() - lasttick);
		lasttick = SDL_GetTicks();
	}
	TTF_CloseFont(font);
	for (GUIThing g : guithings) SDL_FreeSurface(g.s);
	SDL_FreeSurface(guibdr.s);
	QUITGAME;
	logfile.close();
	return 0;
}
