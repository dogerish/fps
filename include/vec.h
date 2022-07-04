#ifndef VECTORSTUFF
#define VECTORSTUFF
#include <string>
#include <sstream>

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
	inline bool operator== (const Vec2d<T> &other)
	{
		return other.x   == this->x &&
		       other.y   == this->y &&
		       other.mag == this->mag;
	}
};

#endif
