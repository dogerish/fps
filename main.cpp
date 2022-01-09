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
#include "guipage.h"
#include "maingui.h"
#include "map.h"
namespace fs = std::filesystem;

#define QUITGAME quit(textures, floortex, ceiltex, ch, window, surface)
#define ERROR_RETURN(code) \
	logfile << SDL_GetError() << std::endl; QUITGAME; \
	logfile.close(); \
	return code;

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

int main(int argc, char* argv[])
{
	// go to the app's directory
	fs::current_path(((fs::path) argv[0]).parent_path());
	// initialize stuff
	SDL_Window*  window;
	SDL_Surface* surface;
	Map* map = create_map(25, 25);
	if (init(window, surface) || TTF_Init()) { ERROR_RETURN(1); }
	GUIPage mgui;
	SDL_SetEventFilter(filter, &mgui.focused);
	// load textures and map
	if (loadtex(textures, floortex, ceiltex, ch)) { ERROR_RETURN(2); }
	std::string mapname = "maze";
	if (loadmap(map, mapname, true)) logfile << SDL_GetError() << std::endl;
	TTF_Font* font = TTF_OpenFont("font.ttf", 14);
	if (font == NULL) { logfile << TTF_GetError() << std::endl; logfile.close(); return 3; }
	char infostr[64]; SDL_Surface* text;
	// set up gui things
	mgui.id = 0;
	maingui_data ptrs;
	mgui.userdata = &ptrs;
	mgui.button_click = mgui_click;
	// reserve enough memory so that the pointers remain valid
	mgui.things.reserve(3);
	// add things and ste up pointers
	ptrs.namebox = addthing(mgui.things, inputbox(font, "Map name:", 100));
	ptrs.savebut = addthing(mgui.things, button(font, "Save"), ALIGN_RIGHT);
	ptrs.loadbut = addthing(mgui.things, button(font, "Load"), ALIGN_LEFT, &ptrs.namebox->r);
	ptrs.mapname = &mapname;
	ptrs.map     = map;
	mgui.bdr     = mapcolumns(mgui.things, font, ptrs);
	// move gui things to center of screen
	{
		SDL_Point offset = {
			(WIDTH  - mgui.bdr.r.w) / 2 - mgui.bdr.r.x,
			(HEIGHT - mgui.bdr.r.h) / 2 - mgui.bdr.r.y
		};
		mgui.bdr.r.x += offset.x; mgui.bdr.r.y += offset.y;
		for (GUIThing& g : mgui.things) { g.r.x += offset.x; g.r.y += offset.y; }
	}
	int showgui = 0;
	// set up variables for game
	Vec2d<float> pos = { map->w / 2.f, map->h / 2.f, -1 };
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
		if (!mgui.focused)
		{
			// positive or negative depending on what keys are pressed
			float multiplier = (kb[SDL_SCANCODE_W] - kb[SDL_SCANCODE_S])
			                   * (float) tdiff / 250.f;
			Vec2d<float> last = pos;
			// update position accounting for heading, and then checking collision
			pos.x += multiplier * fieldcenter.x;
			if (wall_at(map, pos.x, pos.y)->clip) pos.x = last.x;
			pos.y += multiplier * fieldcenter.y;
			if (wall_at(map, pos.x, pos.y)->clip) pos.y = last.y;
			// turning
			heading += (kb[SDL_SCANCODE_D] - kb[SDL_SCANCODE_A]) 
			           * (float) tdiff / 400.f;
			// update fov variables
			fieldleft   = { cos(heading - fov), sin(heading - fov), 1 };
			fieldcenter = { cos(heading),       sin(heading),       1 };
			fieldright  = { cos(heading + fov), sin(heading + fov), 1 };
			// highlight the face under the crosshair
			if (editmode) raycast(pos, fieldcenter, map, hl);
		}
		// render (floor and ceiling) and walls
		renderfloors(surface, pos, fieldleft, fieldright, floortex, ceiltex);
		renderwalls(
			surface,
			pos, heading, editmode,
			map,
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
			SDL_BlitSurface(mgui.bdr.s, NULL, surface, &mgui.bdr.r);
			for (GUIThing g : mgui.things) SDL_BlitSurface(g.s, NULL, surface, &g.r);
		}
		SDL_UpdateWindowSurface(window);

		// process events
		for (SDL_Event e; SDL_PollEvent(&e) && (running = e.type != SDL_QUIT);)
		{
			switch (e.type)
			{
			case SDL_TEXTINPUT: oninput(font, &mgui, e.text.text); break;
			case SDL_KEYDOWN:
			{
				SDL_Keycode keycode = e.key.keysym.sym;
				// gui input
				if (showgui)
				{
					int response = onkeypress(font, &mgui, keycode);
					if (response <= 0) { showgui -= response < 0; break; }
				}
				// texture information for the highlighted tile/face
				Wall* hlwall = editmode ? wall_at(map, hl.x, hl.y) : NULL;
				int facetex = editmode ? face_at(hlwall, hl.mag) : 0;
				int oldtex  = facetex;
				switch (keycode)
				{
				case SDLK_e:      editmode = !editmode; break;
				case SDLK_DOWN:   facetex--; break;
				case SDLK_UP:     facetex++; break;
				case SDLK_ESCAPE: showgui = 1; break;
				}
				// update the texture if needed
				if (editmode && facetex != oldtex)
				{
					facetex = (facetex - 1) % 15 + 1;
					face_at(hlwall, hl.mag, facetex + (facetex <= 0) * 15);
				}
				break;
			}
			case SDL_MOUSEBUTTONDOWN:
				if (!showgui) break;
				showgui -= onclick(font, &mgui, { e.button.x, e.button.y }) < 0;
				break;
			}
		}
		// tick the clock
		tdiff = (SDL_GetTicks() - lasttick);
		lasttick = SDL_GetTicks();
	}
	TTF_CloseFont(font);
	for (GUIThing g : mgui.things) SDL_FreeSurface(g.s);
	SDL_FreeSurface(mgui.bdr.s);
	QUITGAME;
	logfile.close();
	return 0;
}
