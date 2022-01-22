#ifndef UTILS
#define UTILS

#ifdef __linux__
#define SDL2_H <SDL2/SDL.h>
#define SDL2_TTF_H <SDL2/SDL_ttf.h>
#define SDL2_IMG_H <SDL2/SDL_image.h>
#else
#define SDL2_H <SDL.h>
#define SDL2_TTF_H <SDL_ttf.h>
#define SDL2_IMG_H <SDL_image.h>
#endif

#include <cmath>
#include <string>
#include <sstream>
#include SDL2_H

#define TEXSIZE 128
#define SQRT_2  1.4142135624f

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
	{ return other.x == this->x && other.y == this->y && other.mag == this->mag; }
};

#define MAG(vec) vec.mag = hypot(vec.x, vec.y)

int init(SDL_Window* &window, SDL_Surface* &surface, const char* title, int w, int h);

void quit(
	SDL_Surface* titletex,
	SDL_Surface* textures[], SDL_Surface* floortex, SDL_Surface* ceiltex, SDL_Surface* ch,
	SDL_Window* window
);

// load a bmp file and convert to specified format
SDL_Surface* loadwithfmt(const char* file, Uint32 fmt);

// load textures; 0 on success
int loadtex(
	Uint32 fmt,
	SDL_Surface* &titletex,
	SDL_Surface* textures[],
	SDL_Surface* &floortex,
	SDL_Surface* &ceiltex,
	SDL_Surface* &ch
);

#endif
