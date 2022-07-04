#include "utils.h"
#include <SDL.h>
#include <SDL_image.h>
#include <cstdio>
#include <string>
#include <sstream>
#include "game.h"

int init(SDL_Window* &window, SDL_Surface* &surface, const char* title, int w, int h)
{
	int err = 0;
	err |= SDL_Init(SDL_INIT_VIDEO);
	err |= (window = SDL_CreateWindow(
			title,
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h,
			SDL_WINDOW_RESIZABLE
		)) == NULL;
	err |= (surface = SDL_GetWindowSurface(window)) == NULL;
	if (surface) SDL_SetSurfaceRLE(surface, 1);
	return err;
}

void quit(GameTextures &tex, SDL_Window* window)
{
	for (int i = 0; i < 15; i++) SDL_FreeSurface(tex.wall[i]);
	SDL_FreeSurface(tex.floor);
	SDL_FreeSurface(tex.ceiling);
	SDL_FreeSurface(tex.crosshair);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

SDL_Surface* loadwithfmt(
	std::string file, Uint32 fmt, const std::string &resource_path
)
{
	SDL_Surface* tmp = IMG_Load((resource_path + "/" + file).c_str());
	if (!tmp) return NULL;
	SDL_Surface* s = SDL_ConvertSurfaceFormat(tmp, fmt, 0);
	SDL_FreeSurface(tmp);
	return s;
}

// load textures; 0 on success
int loadtex(Uint32 fmt, GameTextures &tex, const std::string &resource_path)
{
	int err = 0;
	tex.title = loadwithfmt("textures/title.png", fmt, resource_path);
	for (int i = 0; i < 15; i++)
	{
		std::stringstream filename;
		filename << "textures/" << (i + 1) << ".png";
		tex.wall[i] = loadwithfmt(filename.str().c_str(), fmt, resource_path);
		err |= tex.wall[i] == NULL;
		if (tex.wall[i]) SDL_SetSurfaceRLE(tex.wall[i], 1);
	}
	tex.floor   = loadwithfmt("textures/floor.png", fmt, resource_path);
	tex.ceiling = loadwithfmt("textures/floor.png", fmt, resource_path);
#define LOCKIFEXISTS(texture) if (texture) \
	{\
		SDL_SetSurfaceRLE(texture, 0);\
		SDL_LockSurface(texture);\
	}
	LOCKIFEXISTS(tex.floor)
	LOCKIFEXISTS(tex.ceiling)
#undef LOCKIFEXISTS
	tex.crosshair = IMG_Load(
		(resource_path + "/textures/crosshair.png").c_str()
	);
	if (tex.crosshair) SDL_SetSurfaceRLE(tex.crosshair, 1);
	return err ||
	       tex.title     == NULL ||
	       tex.floor     == NULL ||
	       tex.ceiling   == NULL ||
	       tex.crosshair == NULL;
}
