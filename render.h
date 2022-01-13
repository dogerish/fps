#ifndef RC_RENDER
#define RC_RENDER
#include "utils.h"
#include "rays.h"
#include SDL2_H
#include "map.h"

void copy_pixel(
	Uint8* srcpx, SDL_Surface* src, SDL_Point srcpt,
	Uint32* dstpx, SDL_Surface* dst, SDL_Point dstpt,
	float vmod
);

void renderfloors(
	SDL_Surface* surface,
	Vec2d<float> pos,
	Vec2d<float> fieldleft, Vec2d<float> fieldright,
	SDL_Surface* floortex, SDL_Surface* ceiltex
);

void renderwalls(
	SDL_Surface* surface,
	Vec2d<float> pos, float heading,
	bool editmode,
	Map* map,
	Vec2d<float> vel /*fieldleft*/, Vec2d<float> fieldright,
	Vec2d<int> hl,
	SDL_Surface* textures[]
);

#endif
