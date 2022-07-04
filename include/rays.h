#ifndef RAYS
#define RAYS
#include "vec.h"
#include "map.h"

// side is stored in tile.mag
Vec2d<float> raycast(
	Vec2d<float> pos, Vec2d<float> vel,
	Map* map,
	Vec2d<int>& tile
);
#endif
