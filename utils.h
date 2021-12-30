#ifndef UTILS
#define UTILS

#ifdef __linux__
#define SDL2_H <SDL2/SDL.h>
#define SDL2_ROTO_H <SDL2/SDL2_rotozoom.h>
#define SDL2_TTF_H <SDL2/SDL_ttf.h>
#endif
#ifdef __APPLE__
#define SDL2_H <SDL.h>
#define SDL2_ROTO_H <SDL2_rotozoom.h>
#define SDL2_TTF_H <SDL_ttf.h>
#endif

#include SDL2_H

#define WIDTH   720
#define HEIGHT  480
#define MAPW    25
#define MAPH    25
#define TEXSIZE 128
#define SQRT_2  1.4142135624f

int init(SDL_Window* &window, SDL_Surface* &surface);

void quit(
	SDL_Surface* textures[], SDL_Surface* floortex, SDL_Surface* ceiltex, SDL_Surface* ch,
	SDL_Window* window, SDL_Surface* surface
);

// load textures; 0 on success
int loadtex(
	SDL_Surface* textures[],
	SDL_Surface* &floortex,
	SDL_Surface* &ceiltex,
	SDL_Surface* &ch
);

#endif
