#include <iostream>
#include <fstream>
#include <filesystem>
#define _USE_MATH_DEFINES
#include <cmath>
#include "utils.h"
#include SDL2_H
#include SDL2_TTF_H
#include "rays.h"
#include "render.h"
namespace fs = std::filesystem;

#define QUITGAME quit(textures, floortex, ceiltex, ch, window, surface)
#define ERROR_RETURN(code) logfile << SDL_GetError() << std::endl; QUITGAME; return code;

#define MAPVER 1
Uint16 tiles[MAPW][MAPH];
SDL_Surface* textures[15];
SDL_Surface* floortex;
SDL_Surface* ceiltex;
SDL_Surface* ch; // crosshair
std::ofstream logfile("logfile.txt", std::ofstream::out);
float fov = (M_PI / 2) / 2;

static int SDLCALL filter(void* arg, SDL_Event* e)
{
	return e->type == SDL_QUIT ||
	       (e->type == SDL_KEYDOWN && !e->key.repeat);
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

int main(int argc, char* argv[])
{
	// go to the app's directory
	fs::current_path(((fs::path) argv[0]).parent_path());
	SDL_Window*  window;
	SDL_Surface* surface;
	if (init(window, surface) || TTF_Init()) { ERROR_RETURN(1); }
	SDL_SetEventFilter(filter, NULL);
	if (loadtex(textures, floortex, ceiltex, ch)) { ERROR_RETURN(1); }
	loadmap("maze", true);
	Vec2d<float> pos = { MAPW / 2.f, MAPH / 2.f, -1};
	float heading = 0;
	Vec2d<float> fieldleft, fieldcenter = { 1, 0, 1 }, fieldright;
	bool editmode = false;
	Vec2d<int> hl;
	TTF_Font* font = TTF_OpenFont("Sans.ttf", 12);
	if (!font) logfile << SDL_GetError() << std::endl;
	SDL_Surface* text;
	char infostr[64];
	Uint32 lasttick = SDL_GetTicks();
	const Uint8* kb = SDL_GetKeyboardState(NULL);
	bool gotevent = false;
	for (SDL_Event e; e.type != SDL_QUIT; gotevent = SDL_PollEvent(&e))
	{
		float tdiff = (SDL_GetTicks() - lasttick);
		lasttick = SDL_GetTicks();
		// update player
		{
			float multiplier = (kb[SDL_SCANCODE_W] - kb[SDL_SCANCODE_S]) / 250.f;
			Vec2d<float> last = pos;
			pos.x += multiplier * tdiff * fieldcenter.x;
			if (tiles[(int) pos.y][(int) pos.x]) pos.x = last.x;
			pos.y += multiplier * tdiff * fieldcenter.y;
			if (tiles[(int) pos.y][(int) pos.x]) pos.y = last.y;
			heading += (kb[SDL_SCANCODE_D] - kb[SDL_SCANCODE_A]) * tdiff / 400.f;
			fieldleft   = { cos(heading - fov), sin(heading - fov), 1 };
			fieldcenter = { cos(heading),       sin(heading),       1 };
			fieldright  = { cos(heading + fov), sin(heading + fov), 1 };
			// highlight the face under the crosshair
			if (editmode) raycast(pos, fieldcenter, tiles, hl);
		}
		// render
		renderfloors(surface, pos, fieldleft, fieldright, floortex, ceiltex);
		renderwalls(surface, pos, heading, editmode, tiles, fieldleft, fieldright, hl, textures);
		// info text
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
		// crosshair
		rect = { (WIDTH - ch->w) / 2, (HEIGHT - ch->h) / 2, ch->w, ch->h };
		SDL_BlitSurface(ch, NULL, surface, &rect);

		SDL_UpdateWindowSurface(window);
		SDL_FreeSurface(text);

		// process key event
		if (gotevent && e.type == SDL_KEYDOWN)
		{
			Uint16 current = editmode ? tiles[hl.y][hl.x] : 0;
			Uint16 facetex = current >> hl.mag * 4 & 0xf;
			Uint16 oldtex  = facetex;
			switch (e.key.keysym.sym)
			{
			case SDLK_e:    editmode = !editmode; break;
			case SDLK_DOWN: facetex--; break;
			case SDLK_UP:   facetex++; break;
			case SDLK_o:    savemap("maze"); break;
			}
			// update the face
			if (editmode && facetex != oldtex)
			{
				facetex = (facetex - 1) % 15 + 1;
				facetex += (facetex <= 0) * 15;
				tiles[hl.y][hl.x] = current & ~(0xf << hl.mag * 4)
				                    | facetex << hl.mag * 4;
			}
		}
	}
	TTF_CloseFont(font);
	QUITGAME;
	logfile.close();
	return 0;
}
