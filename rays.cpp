#include "rays.h"
#include <iostream>
#ifdef __linux__
#include <SDL2/SDL.h>
#endif
#ifdef __APPLE__
#include <SDL.h>
#endif
#include <cmath>
#include "utils.h"

// side is stored in tile.mag
Vec2d<float> raycast(
	Vec2d<float> pos, Vec2d<float> vel,
	const Uint16 tiles[][MAPW],
	Vec2d<int>& tile
)
{
	Vec2d<float> original = pos;
	Vec2d<float> dx, dy;
	Vec2d<float>* axes[] = { &dx, &dy };
	// adjust for which way the ray is going
	dx.x = ((int) pos.x + (vel.x > 0)) - pos.x;
	dy.y = ((int) pos.y + (vel.y > 0)) - pos.y;
	// calculate alternate axis values
	dx.y = dx.x * vel.y / vel.x;
	dy.x = dy.y * vel.x / vel.y;
	// calculate magnitudes
	MAG(dx); MAG(dy);
	float tmag = 0;
	// first time for axis, do add step on whatever iteration
	bool firstof = true, doAdd = true;
	// start with the smaller magnitude
	int axis = dy.mag < dx.mag || isnan(dx.mag);
	if (isnan(axes[axis]->mag))
	{
		*axes[axis] = { 0, 0, 0};
		vel.y = -1;
	}
	Vec2d<float> next = { pos.x + axes[!axis]->x, pos.y + axes[!axis]->y, axes[!axis]->mag };
	while (true)
	{
		if (doAdd)
		{
			pos.x += axes[axis]->x;
			pos.y += axes[axis]->y;
			tmag  += axes[axis]->mag;
		}
		else doAdd = true;
		tile = {
			(int) (pos.x - (!axis && vel.x < 0)),
			(int) (pos.y - ( axis && vel.y < 0)),
			-1
		};
		if (
			// check if tile is out of map
			   (MAPW <= tile.x) || 0 > tile.x
			|| (MAPH <= tile.y) || 0 > tile.y
			// check if tile is filled
			|| tiles[tile.y][tile.x]
		)
		{
			tile.mag = axis + (axis ? vel.y < 0 : vel.x < 0) * 2;
			Vec2d<float> d = { pos.x - original.x, pos.y - original.y, -1 };
			MAG(d);
			return d;
		}
		if (firstof)
		{
			// adjust for one tile in the direction
			dx.x = (vel.x > 0) * 2 - 1;
			dy.y = (vel.y > 0) * 2 - 1;
			// calculate alternate axis values
			dx.y = dx.x * vel.y / vel.x;
			dy.x = dy.y * vel.x / vel.y;
			MAG(dx); MAG(dy);
			firstof = false;
		}
		// use other axis when this one is going to surpass it
		if (tmag + axes[axis]->mag > next.mag)
		{
			Vec2d tmp = pos;
			pos = next;
			pos.mag = -1;
			tmag = next.mag - tmag;
			next.x = tmp.x + axes[axis]->x;
			next.y = tmp.y + axes[axis]->y;
			next.mag = axes[axis]->mag;
			axis = !axis;
			// don't add again on next iteration
			doAdd = false;
		}
	}
}
