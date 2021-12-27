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

int init(SDL_Window** window, SDL_Renderer** renderer);
void quit(
	SDL_Texture* textures[], SDL_Texture* floortex, SDL_Texture* ceiltex,
	SDL_Window* window, SDL_Renderer* renderer
);

int loadtex(
	SDL_Renderer* renderer,
	SDL_Texture* textures[],
	SDL_Texture* &floortex,
	SDL_Texture* &ceiltex
);
#endif
