#ifndef RC_RENDER
#define RC_RENDER
#include <SDL.h>
#include <SDL_ttf.h>
#include "vec.h"
#include "map.h"
#include "game.h"

void drawfps(GameData& gd);

void renderfloors(GameData& gd, SDL_Surface* floortex, SDL_Surface* ceiltex);

void renderwalls(GameData& gd, SDL_Surface* textures[]);

#endif
