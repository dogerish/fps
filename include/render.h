#ifndef RC_RENDER
#define RC_RENDER
#include <SDL.h>
#include <SDL_ttf.h>
#include "vec.h"
#include "map.h"

void drawfps(SDL_Surface* surface, TTF_Font* font, float fps);

void renderfloors(
	SDL_Surface* surface,
	Vec2d<float> pos,
	Vec2d<float> fieldleft, Vec2d<float> fieldright,
	SDL_Surface* floortex, SDL_Surface* ceiltex
);

void renderwalls(
	SDL_Surface* surface,
	Vec2d<float> pos,
	bool editmode,
	Map* map,
	Vec2d<float> vel /*fieldleft*/, Vec2d<float> fieldright,
	Vec2d<int> hl,
	SDL_Surface* textures[]
);

#endif
