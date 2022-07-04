#ifndef UTILS
#define UTILS
#include <cmath>
#include <string>
#include <sstream>
#include <SDL.h>
#include "game.h"

#define TEXSIZE 128
#define SQRT_2  1.4142135624f

#define MAG(vec) vec.mag = hypot(vec.x, vec.y)

int init(SDL_Window* &window, SDL_Surface* &surface, const char* title, int w, int h);

void quit(GameTextures &tex, SDL_Window* window);

// load a bmp file and convert to specified format
SDL_Surface* loadwithfmt(
	std::string file, Uint32 fmt, const std::string &resource_path
);

// load textures; 0 on success
int loadtex(Uint32 fmt, GameTextures &tex, const std::string &resource_path);

#endif
