#ifndef RAYS
#define RAYS
#ifdef __linux__
#include <SDL2/SDL.h>
#endif
#ifdef __APPLE__
#include <SDL.h>
#endif
#include <cmath>
#include <string>
#include <sstream>
#include "utils.h"

template<typename T> struct Vec2d
{
	T x;
	T y;
	T mag = -1;
	operator std::string()
	{
		std::stringstream ss;
		ss << "(" << this->x << ", " << this->y << ")";
		return ss.str();
	}
};

#define MAG(vec) vec.mag = hypot(vec.x, vec.y)

Vec2d<float> raycast(
	Vec2d<float> pos, Vec2d<float> vel,
	const Uint16 tiles[][MAPW],
	int& side, Vec2d<int>& tile
);
#endif
