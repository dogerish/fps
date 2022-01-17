#ifndef UTILS
#define UTILS

#ifdef __linux__
#define SDL2_H <SDL2/SDL.h>
#define SDL2_TTF_H <SDL2/SDL_ttf.h>
#define SDL2_IMG_H <SDL2/SDL_image.h>
#else
#define SDL2_H <SDL.h>
#define SDL2_TTF_H <SDL_ttf.h>
#define SDL2_IMG_H <SDL_image.h>
#endif

#include SDL2_H

#define WIDTH   720
#define HEIGHT  480
#define TEXSIZE 128
#define SQRT_2  1.4142135624f

int init(SDL_Window* &window, SDL_Surface* &surface);

void quit(
	SDL_Surface* textures[], SDL_Surface* floortex, SDL_Surface* ceiltex, SDL_Surface* ch,
	SDL_Window* window
);

// load a bmp file and convert to specified format
SDL_Surface* loadwithfmt(const char* file, Uint32 fmt);

// load textures; 0 on success
int loadtex(
	Uint32 fmt,
	SDL_Surface* textures[],
	SDL_Surface* &floortex,
	SDL_Surface* &ceiltex,
	SDL_Surface* &ch
);

#endif
