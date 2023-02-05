#include <iostream>
#include <fstream>
#include <vector>
#define _USE_MATH_DEFINES
#include <cmath>
#include "unistd.h"
#include "utils.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include "rays.h"
#include "render.h"
#include "gui.h"
#include "guipage.h"
#include "maingui.h"
#include "vec.h"
#include "map.h"
#include "game.h"

static int SDLCALL filter(void* userdata, SDL_Event* e)
{
	switch (e->type)
	{
		case SDL_QUIT:
		case SDL_TEXTINPUT:
		case SDL_KEYDOWN:
		case SDL_MOUSEBUTTONDOWN:
			return 1;
		case SDL_WINDOWEVENT:
			return e->window.event == SDL_WINDOWEVENT_SIZE_CHANGED;
		default:
			return 0;
	}
}

#define QUITGAME quit(tex, gd.window)
#define ERROR_RETURN(code, lib) \
	logfile << lib##_GetError() << std::endl; QUITGAME; \
	logfile.close(); \
	return code;

void blit_centered(SDL_Surface* src, SDL_Surface* dst)
{
	SDL_Rect r = { (dst->w - src->w) / 2, (dst->h - src->h) / 2, src->w, src->h };
	SDL_BlitSurface(src, NULL, dst, &r);
}

const char* USAGE = "raycaster [-Rresource_directory]\n";

int main(int argc, char* argv[])
{
	// parse command line options
	GameData gd;
	GameTextures tex;
	int o;
	while ((o = getopt(argc, argv, "R:")) != -1)
	{
		switch (o)
		{
			case '?':
				std::cout << USAGE;
				return -1;
			case 'R':
				gd.resource_path = optarg;
				break;
		}
	}

	std::ofstream logfile("logfile.txt", std::ofstream::out);
	// initialize stuff
	if (init(gd.window, gd.surface, "WalkerPonk 2069", 720, 480)) { ERROR_RETURN(1, SDL); }
	if (TTF_Init()) { ERROR_RETURN(1, TTF); }
	if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) { ERROR_RETURN(1, IMG); }
	SDL_SetEventFilter(filter, NULL);
	// load textures and map
	if (loadtex(gd.surface->format->format, tex, gd.resource_path))
	{
		if (SDL_GetError()[0]) { ERROR_RETURN(1, SDL); }
		if (IMG_GetError()[0]) { ERROR_RETURN(1, IMG); }
	}
	gd.font = TTF_OpenFont((gd.resource_path + "/font.ttf").c_str(), 14);
	if (gd.font == NULL) { logfile << TTF_GetError() << std::endl; logfile.close(); return 3; }
	// game vars
	gd.map = create_map(25, 25);
	gd.pos = { gd.map->w / 2.f, gd.map->h / 2.f, 0 };
	gd.fov = (M_PI / 2) / 2;
	// editing variables
	gd.gamemode = GM_TITLESCREEN;
	gd.editmode = false; gd.hl = { 0, 0, 0 };
	// set up gui things
	const int NUMPAGES = 4;
	GUIPage* guipages[NUMPAGES];
	guipages[0] = new  TitleGUI(gd, 0);
	guipages[1] = new MapSelGUI(gd, 1);
	guipages[2] = new   EditGUI(gd, 2);
	guipages[3] = new   WallGUI(gd, 3);
	gd.page = 0;
	SDL_StopTextInput();
	// main loop variables
	Uint32 lasttick = SDL_GetTicks(), tdiff = 0, lastfpstick = 0;
	float fps = 0; int fpsdelay = 100, framecount = 0;
	const Uint8* kb = SDL_GetKeyboardState(NULL);
	bool running = true;
	// main loop
	while (running)
	{
		if (gd.gamemode != GM_TITLESCREEN && gd.map->loaded)
		{
			if (gd.editmode && gd.gamemode != GM_EDITING) gd.editmode = false;
			// update player
			if (gd.page < 0 || !guipages[gd.page]->focused)
			{
				// positive or negative depending on what keys are pressed
				float multiplier = (kb[SDL_SCANCODE_W] - kb[SDL_SCANCODE_S])
						   * (float) tdiff / 250.f;
				Vec2d<float> last = gd.pos;
				// update position and then check collision
				gd.pos.x += multiplier * gd.fieldcenter.x;
				if (wall_at(gd.map, floor(gd.pos.x), floor(gd.pos.y))->clip)
					gd.pos.x = last.x;
				gd.pos.y += multiplier * gd.fieldcenter.y;
				if (wall_at(gd.map, floor(gd.pos.x), floor(gd.pos.y))->clip)
					gd.pos.y = last.y;
				// turning
				gd.pos.mag += (kb[SDL_SCANCODE_D] - kb[SDL_SCANCODE_A])
					   * (float) tdiff / 400.f;
			}
			// update fov variables
			gd.fieldleft   = { cos(gd.pos.mag - gd.fov), sin(gd.pos.mag - gd.fov), 1 };
			gd.fieldcenter = { cos(gd.pos.mag),          sin(gd.pos.mag),          1 };
			gd.fieldright  = { cos(gd.pos.mag + gd.fov), sin(gd.pos.mag + gd.fov), 1 };
			// highlight the face under the crosshair
			if (gd.editmode) raycast(gd.pos, gd.fieldcenter, gd.map, gd.hl);
			// render (floor and ceiling) and walls
			renderfloors(
				gd.surface,
				gd.pos,
				gd.fieldleft, gd.fieldright,
				tex.floor, tex.ceiling
			);
			renderwalls(
				gd.surface,
				gd.pos, gd.editmode,
				gd.map,
				gd.fieldleft, gd.fieldright,
				gd.hl,
				tex.wall
			);
		}
		else
		{
			// draw title/home screen
			SDL_FillRect(gd.surface, NULL, *(Uint32*) tex.title->pixels);
			blit_centered(tex.title, gd.surface);
			if (gd.page < 0) gd.page = 0;
		}
		// crosshair
		if (gd.gamemode != GM_TITLESCREEN && gd.map->loaded)
			blit_centered(tex.crosshair, gd.surface);
		// gui
		if (gd.page >= 0) guipages[gd.page]->drawgui(gd, tdiff);
		drawfps(gd.surface, gd.font, fps);
		SDL_UpdateWindowSurface(gd.window);

		// process events
		for (SDL_Event e; SDL_PollEvent(&e) && (running = e.type != SDL_QUIT);)
		{
			switch (e.type)
			{
			case SDL_TEXTINPUT: guipages[gd.page]->oninput(gd, e.text.text); break;
			case SDL_KEYDOWN:
			{
				SDL_Keycode keycode = e.key.keysym.sym;
				// gui input
				if (gd.page >= 0)
				{
					int r = guipages[gd.page]->onkeypress(gd, keycode);
					if (r <= 0)
					{
						if (r < 0) guipages[gd.page]->closegui(gd);
						break;
					}
				}
				int facedelta = 0;
				switch (keycode)
				{
				case SDLK_e:
					gd.editmode = gd.gamemode == GM_EDITING && !gd.editmode;
					break;
				case SDLK_DOWN: facedelta = -1; break;
				case SDLK_UP:   facedelta = +1; break;
				case SDLK_ESCAPE:
					guipages[0]->opengui(gd);
					break;
				}
				// update the texture if needed
				if (gd.editmode && facedelta)
				{
					Wall* hlwall = wall_at(gd.map, gd.hl.x, gd.hl.y);
					int facetex  = face_at(hlwall, gd.hl.mag) + facedelta;
					facetex = (facetex - 1) % 15 + 1;
					face_at(hlwall, gd.hl.mag, facetex + (facetex <= 0) * 15);
				}
				break;
			}
			case SDL_MOUSEBUTTONDOWN:
			{
				if (gd.page < 0) break;
				int r = guipages[gd.page]->onclick(gd, { e.button.x, e.button.y });
				// decode response
				if (!r) break;
				else if (r > 1 && r - 2 < NUMPAGES)
					guipages[r - 2]->opengui(gd);
				// change gui without writing history
				else if (r < -1 && -2 - r < NUMPAGES) gd.page = -2 - r;
				else if (r == -1) guipages[gd.page]->closegui(gd);
				break;
			}
			case SDL_WINDOWEVENT:
				gd.surface = SDL_GetWindowSurface(gd.window);
				// re-center all gui pages
				for (GUIPage* p : guipages)
					p->center_page(gd);
				break;
			}
		}
		// tick the clock
		tdiff = (SDL_GetTicks() - lasttick);
		lasttick = SDL_GetTicks();
		framecount++;
		if (lasttick >= lastfpstick + fpsdelay)
		{
			fps = framecount * 1000.f / (lasttick - lastfpstick);
			lastfpstick = lasttick;
			framecount = 0;
		}
	}

	TTF_CloseFont(gd.font);
	for (GUIPage* p : guipages)
	{
		for (GUIThing &g : p->things) SDL_FreeSurface(g.s);
		SDL_FreeSurface(p->bdr.s);
	}
	delete (wallgui_data*)   guipages[3]->userdata;
	for (int i = 0; i < NUMPAGES; i++)
		delete guipages[i];
	free_map(gd.map);
	QUITGAME;
	logfile.close();
	return 0;
}
