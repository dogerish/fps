#include <iostream>
#include <fstream>
#include <vector>
#include <string>
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
#include "command.h"

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

#define QUITGAME quit(gd, tex)
#define ERROR_RETURN(code, lib) \
	gd.log(lib##_GetError()); QUITGAME; \
	return code;

void blit_centered(SDL_Surface* src, SDL_Surface* dst)
{
	SDL_Rect r = { (dst->w - src->w) / 2, (dst->h - src->h) / 2, src->w, src->h };
	SDL_BlitSurface(src, NULL, dst, &r);
}

const char* USAGE = "raycaster [-Rresource_directory]\n";

GameData gd;
GameTextures tex;
const int NUMPAGES = 5;
enum GUIPageID {
	GP_NONE = -1,
	GP_TITLE = 0,
	GP_MAPSEL = 1,
	GP_EDIT = 2,
	GP_WALL = 3,
	GP_COMMAND = 4,
};
GUIPage* guipages[NUMPAGES];

/* -- COMMAND FUNCTIONS -- */
#define CALLARGVRES(res, args...) gd.cmdhand->call(std::vector<std::string> { args }, res)
#define CALLARGV(args...) gd.cmdhand->call(std::vector<std::string> { args })
#define CMDDEF(name, body) \
	unsigned int cmdfun_ ## name (const std::vector<std::string> &argv, std::string &result) \
	body \
	Command cmd_ ## name = { #name, cmdfun_ ## name };

	/* - VARIABLES - */

	/* gamemode [titlescreen|playing|editing] */
CMDDEF(gamemode, 
{
	if (argv.size() > 1)
	{
		std::string n = argv[1];
		gd.map->loaded = gd.gamemode != GM_TITLESCREEN;
		if      (n == "titlescreen")
		{
			gd.gamemode = GM_TITLESCREEN;
			gd.map->loaded = false;
		}
		else if (n == "playing") gd.gamemode = GM_PLAYING;
		else if (n == "editing") gd.gamemode = GM_EDITING;
		else
		{
			gd.log("Unknown gamemode");
			return 1;
		}
		// if mode needs map but doens't have one, select map
		if ((gd.gamemode == GM_PLAYING || gd.gamemode == GM_EDITING) && !gd.map->loaded)
		{
			CALLARGV("closegui");
			CALLARGV("showgui", "mapsel");
		}
	}
	switch (gd.gamemode)
	{
	case GM_TITLESCREEN: result = "titlescreen"; break;
	case GM_PLAYING: result = "playing"; break;
	case GM_EDITING: result = "editing"; break;
	}
	return 0;
})

	/* editguimapname [<name>] */
CMDDEF(editguimapname,
{
	GUIThing &t = guipages[GP_EDIT]->things[EditGUI::EGNAME];
	if (argv.size() > 1)
	{
		t.value = argv[1];
		redrawinput(gd, &t);
	}
	result = t.value;
	return 0;
})

	/* commandguicmd [<cmd>] */
CMDDEF(commandguicmd,
{
	GUIThing &t = guipages[GP_COMMAND]->things[CommandGUI::ID_CMD];
	if (argv.size() > 1)
	{
		t.value = argv[1];
		redrawinput(gd, &t);
	}
	result = t.value;
	return 0;
})

	/* - COMMANDS - */

	/* echo <text> */
CMDDEF(echo, 
{
	if (argv.size() < 2) gd.log(result = "");
	else gd.log(result = argv[1]);
	return 0;
})

	/* eval <cmdstr> */
CMDDEF(eval, 
{
	if (argv.size() < 2) return 0;
	return gd.cmdhand->run(argv[1], result);
})

	/* showgui <guiname> */
CMDDEF(showgui, 
{
	if (argv.size() < 2) return 1;
	std::string name = argv[1];
	for (int i = 0; i < NUMPAGES; i++)
	{
		if (guipages[i]->name == name)
		{
			guipages[i]->opengui(gd);
			return 0;
		}
	}
	gd.log("Unknown gui page");
	return 1;
})

	/* closegui */
CMDDEF(closegui, 
{
	if (gd.page >= 0) guipages[gd.page]->closegui(gd);
	return 0;
})

	/* refreshgui <guiname> */
CMDDEF(refreshgui, 
{
	if (argv.size() < 2) return 1;
	std::string name = argv[1];
	for (int i = 0; i < NUMPAGES; i++)
	{
		if (guipages[i]->name == name)
		{
			guipages[i]->refresh(gd);
			return 0;
		}
	}
	gd.log("Unknown gui page");
	return 1;
})

	/* savemap <name> */
CMDDEF(savemap, 
{
	if (argv.size() < 2)
	{
		gd.log("Saving failed; no name given.");
		return 2;
	}
	if (savemap(gd.map, argv[1], gd.resource_path))
	{
		gd.log("Saving failed.");
		return 1;
	}
	guipages[GP_EDIT]->refresh(gd);
	return 0;
})

	/* loadmap <name> */
CMDDEF(loadmap, 
{
	if (argv.size() < 2)
	{
		gd.log("Loading failed; no name given.");
		return 2;
	}
	if (loadmap(gd.map, gd.pos, argv[1], gd.resource_path))
	{
		gd.log("Loading failed.");
		return 1;
	}
	return 0;
})

	/* newmap */
CMDDEF(newmap, 
{
	newmap(gd.map, gd.pos);
	return 0;
})

	/* quit */
CMDDEF(quit, 
{
	SDL_Event e; e.type = SDL_QUIT;
	SDL_PushEvent(&e);
	return 0;
})

	/* wall <x> <y> [<value>] */
CMDDEF(wall, 
{
	if (argv.size() < 3)
	{
		gd.log("Not enough arguments.");
		return 1;
	}
	int x = CommandHandler::parseint(argv[1], -1);
	int y = CommandHandler::parseint(argv[2], -1);
	if (x < 0 || x > gd.map->w || y < 0 || y > gd.map->h)
	{
		gd.log("Coordinates out of range.");
		return 2;
	}
	int i = x + y * gd.map->w;
	GUIThing &t = guipages[GP_WALL]->things[WallGUI::WG_WALLSTART + i];
	if (argv.size() > 3)
	{
		int action = CommandHandler::parseint(argv[3], -1);
		if (action < 0)
			gd.map->data[i].clip = t.overflown = !gd.map->data[i].clip;
		else
			gd.map->data[i].clip = t.overflown = (bool) action;
		if (gd.map->data[i].n == 0) set_faces(&gd.map->data[i], 1, 1, 1, 1);
		WallGUI::button_update(gd, &t);
	}
	result = std::to_string(gd.map->data[i].clip);
	return 0;
})

int main(int argc, char* argv[])
{
	std::vector<Command> cmdv = {
		cmd_gamemode,
		cmd_editguimapname,
		cmd_commandguicmd,
		cmd_echo,
		cmd_eval,
		cmd_showgui,
		cmd_closegui,
		cmd_refreshgui,
		cmd_savemap,
		cmd_loadmap,
		cmd_newmap,
		cmd_quit,
		cmd_wall
	};
	gd.cmdhand = new CommandHandler(cmdv);
	// parse command line options
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

	// initialize stuff
	if (init(gd, "WalkerPonk 2069", 720, 480)) { ERROR_RETURN(1, SDL); }
	if (TTF_Init()) { ERROR_RETURN(1, TTF); }
	if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) { ERROR_RETURN(1, IMG); }
	SDL_SetEventFilter(filter, NULL);
	// load textures and map
	if (loadtex(gd, gd.surface->format->format, tex))
	{
		if (SDL_GetError()[0]) { ERROR_RETURN(1, SDL); }
		if (IMG_GetError()[0]) { ERROR_RETURN(1, IMG); }
	}
	gd.font = TTF_OpenFont((gd.resource_path + "/font.ttf").c_str(), 14);
	if (gd.font == NULL) { ERROR_RETURN(3, TTF); }
	// game vars
	gd.map = create_map(25, 25);
	gd.pos = { gd.map->w / 2.f, gd.map->h / 2.f, 0 };
	gd.fov = (M_PI / 2) / 2;
	// editing variables
	gd.gamemode = GM_TITLESCREEN;
	gd.editmode = false; gd.hl = { 0, 0, 0 };
	guipages[GP_TITLE] = new TitleGUI(gd, GP_TITLE, "title");
	guipages[GP_MAPSEL] = new MapSelGUI(gd, GP_MAPSEL, "mapsel");
	guipages[GP_EDIT] = new EditGUI(gd, GP_EDIT, "edit");
	guipages[GP_WALL] = new WallGUI(gd, GP_WALL, "wall");
	guipages[GP_COMMAND] = new CommandGUI(gd, GP_COMMAND, "command");
	gd.page = GP_TITLE;
	SDL_StopTextInput();
	// main loop variables
	Uint32 lasttick = SDL_GetTicks(), tdiff = 0, lastfpstick = 0;
	int fpsdelay = 100, framecount = 0;
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
			renderfloors(gd, tex.floor, tex.ceiling);
			renderwalls(gd, tex.wall);
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
		drawfps(gd);
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
					guipages[GP_TITLE]->opengui(gd);
					break;
				case SDLK_SLASH:
					guipages[GP_COMMAND]->opengui(gd);
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
				guipages[gd.page]->onclick(gd, { e.button.x, e.button.y });
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
			gd.fps = framecount * 1000.f / (lasttick - lastfpstick);
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
	return 0;
}
