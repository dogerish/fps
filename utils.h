#ifndef UTILS
#define UTILS

#ifdef __linux__
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#endif
#ifdef __APPLE__
#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#endif

#define WIDTH   720
#define HEIGHT  480
#define MAPW    25
#define MAPH    25
#define TEXSIZE 64
#define SQRT_2  1.4142135624f

int init(SDL_Window* &window, SDL_Surface* &surface);

void quit(
	SDL_Surface* textures[], SDL_Surface* floortex, SDL_Surface* ceiltex,
	SDL_Window* window, SDL_Surface* surface
);

// load textures; 0 on success
int loadtex(
	SDL_Surface* textures[],
	SDL_Surface* &floortex,
	SDL_Surface* &ceiltex
);

#endif
