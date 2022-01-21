#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#define _USE_MATH_DEFINES
#include <cmath>
#include "utils.h"
#include SDL2_H
#include SDL2_TTF_H
#include SDL2_IMG_H
#include "rays.h"
#include "render.h"
#include "gui.h"
#include "guipage.h"
#include "maingui.h"
#include "map.h"
namespace fs = std::filesystem;

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

#define QUITGAME quit(titletex, textures, floortex, ceiltex, ch, window)
#define ERROR_RETURN(code, lib) \
	logfile << lib##_GetError() << std::endl; QUITGAME; \
	logfile.close(); \
	return code;

void blit_centered(SDL_Surface* src, SDL_Surface* dst)
{
	SDL_Rect r = { (dst->w - src->w) / 2, (dst->h - src->h) / 2, src->w, src->h };
	SDL_BlitSurface(src, NULL, dst, &r);
}

int main(int argc, char* argv[])
{
	// go to the app's directory
	fs::current_path(((fs::path) argv[0]).parent_path());
	std::ofstream logfile("logfile.txt", std::ofstream::out);
	SDL_Surface* titletex;
	SDL_Surface* textures[15];
	SDL_Surface* floortex;
	SDL_Surface* ceiltex;
	SDL_Surface* ch; // crosshair
	// initialize stuff
	SDL_Window*  window = NULL;
	SDL_Surface* surface = NULL;
	if (init(window, surface, "WalkerPonk 2069", 720, 480)) { ERROR_RETURN(1, SDL); }
	int draw_w = surface->w, draw_h = surface->h;
	if (TTF_Init()) { ERROR_RETURN(1, TTF); }
	if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) { ERROR_RETURN(1, IMG); }
	Map* map = create_map(25, 25);
	SDL_SetEventFilter(filter, map);
	// load textures and map
	if (loadtex(surface->format->format, titletex, textures, floortex, ceiltex, ch))
	{
		if (SDL_GetError()[0]) { ERROR_RETURN(1, SDL); }
		if (IMG_GetError()[0]) { ERROR_RETURN(1, IMG); }
	}
	TTF_Font* font = TTF_OpenFont("font.ttf", 14);
	if (font == NULL) { logfile << TTF_GetError() << std::endl; logfile.close(); return 3; }
	// set up variables for game
	Vec2d<float> pos = { map->w / 2.f, map->h / 2.f, -1 };
	float heading = 0, fov = (M_PI / 2) / 2;
	// normalized coordinates from leftmost fov to rightmost
	Vec2d<float> fieldleft, fieldcenter = { 1, 0, 1 }, fieldright;
	// editing variables
	int gamemode = GM_TITLESCREEN;
	bool editmode = false; Vec2d<int> hl = { 0, 0, 0 };
	// set up gui things
	const int NUMPAGES = 4;
	GUIPage guipages[NUMPAGES];
	setup_titlegui(guipages[0], font, draw_w, draw_h, &gamemode, map);
	setup_mapselgui(guipages[1], font, draw_w, draw_h, &gamemode, map);
	setup_editgui(guipages[2], font, draw_w, draw_h, map);
	setup_wallgui(guipages[3], font, draw_w, draw_h, map, &pos, &fieldcenter, &editmode, &hl);
	GUIPage* showgui = &guipages[0];
	std::vector<GUIPage*> pagehistory;
	SDL_StopTextInput();
	// main loop variables
	Uint32 lasttick = SDL_GetTicks(), tdiff = 0, lastfpstick = 0;
	float fps = 0; int fpsdelay = 100; int framecount = 0;
	const Uint8* kb = SDL_GetKeyboardState(NULL);
	bool running = true;
	// main loop
	while (running)
	{
		if (gamemode != GM_TITLESCREEN && map->loaded)
		{
			if (editmode && gamemode != GM_EDITING) editmode = false;
			// update player
			if (!showgui || !showgui->focused)
			{
				// positive or negative depending on what keys are pressed
				float multiplier = (kb[SDL_SCANCODE_W] - kb[SDL_SCANCODE_S])
						   * (float) tdiff / 250.f;
				Vec2d<float> last = pos;
				// update position and then check collision
				pos.x += multiplier * fieldcenter.x;
				if (wall_at(map, floor(pos.x), floor(pos.y))->clip) pos.x = last.x;
				pos.y += multiplier * fieldcenter.y;
				if (wall_at(map, floor(pos.x), floor(pos.y))->clip) pos.y = last.y;
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
		}
		else
		{
			// draw title/home screen
			SDL_FillRect(surface, NULL, *(Uint32*) titletex->pixels);
			blit_centered(titletex, surface);
			if (!showgui) showgui = &guipages[0];
		}
		// crosshair
		if (gamemode != GM_TITLESCREEN && map->loaded) blit_centered(ch, surface);
		// gui
		if (showgui) drawgui(surface, showgui, font, tdiff);
		drawfps(surface, font, fps);
		SDL_UpdateWindowSurface(window);

		// process events
		for (SDL_Event e; SDL_PollEvent(&e) && (running = e.type != SDL_QUIT);)
		{
			switch (e.type)
			{
			case SDL_TEXTINPUT: oninput(font, showgui, e.text.text); break;
			case SDL_KEYDOWN:
			{
				SDL_Keycode keycode = e.key.keysym.sym;
				// gui input
				if (showgui)
				{
					int r = onkeypress(font, showgui, keycode);
					if (r <= 0)
					{
						if (r < 0) closegui(showgui, pagehistory);
						break;
					}
				}
				// texture information for the highlighted tile/face
				Wall* hlwall = editmode ? wall_at(map, hl.x, hl.y) : NULL;
				int facetex = editmode ? face_at(hlwall, hl.mag) : 0;
				int oldtex  = facetex;
				switch (keycode)
				{
				case SDLK_e: editmode = gamemode == GM_EDITING && !editmode; break;
				case SDLK_DOWN: facetex--; break;
				case SDLK_UP:   facetex++; break;
				case SDLK_ESCAPE:
					opengui(showgui, &guipages[0], pagehistory);
					break;
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
			{
				if (!showgui) break;
				int r = onclick(font, showgui, { e.button.x, e.button.y });
				// decode response
				if (!r) break;
				else if (r > 1 && r - 2 < NUMPAGES)
					opengui(showgui, &guipages[r - 2], pagehistory);
				// change gui without writing history
				else if (r < -1 && -2 - r < NUMPAGES) showgui = &guipages[-2 - r];
				else if (r == -1) closegui(showgui, pagehistory);
				break;
			}
			case SDL_WINDOWEVENT:
				surface = SDL_GetWindowSurface(window);
				draw_w = surface->w, draw_h = surface->h;
				// re-center all gui pages
				for (GUIPage &p : guipages)
					center_page(p, { draw_w / 2, draw_h / 2 });
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

	TTF_CloseFont(font);
	for (GUIPage &p : guipages)
	{
		for (GUIThing &g : p.things) SDL_FreeSurface(g.s);
		SDL_FreeSurface(p.bdr.s);
	}
	delete (titlegui_data*) guipages[0].userdata;
	delete (editgui_data*)  guipages[1].userdata;
	delete (wallgui_data*)  guipages[2].userdata;
	free_map(map);
	QUITGAME;
	logfile.close();
	return 0;
}
