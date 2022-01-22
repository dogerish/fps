#ifndef RAYS
#define RAYS
#include "utils.h"
#include "map.h"
#include SDL2_H

// side is stored in tile.mag
Vec2d<float> raycast(
	Vec2d<float> pos, Vec2d<float> vel,
	Map* map,
	Vec2d<int>& tile
);
#endif
