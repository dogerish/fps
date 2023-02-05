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

int init(GameData &gd, const char* title, int w, int h);

void quit(GameData &gd, GameTextures &tex);

// load a bmp file and convert to specified format
SDL_Surface* loadwithfmt(GameData &gd, std::string file, Uint32 fmt);

// load textures; 0 on success
int loadtex(GameData &gd, Uint32 fmt, GameTextures &tex);

#endif
