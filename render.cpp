#include "render.h"
#include <cmath>
#include "utils.h"
#include SDL2_H
#include "rays.h"
#include "map.h"

#define MAXDIST 30.f
// value to use for color mod to give fog
#define FOGMOD(dist) (0xff - 0xef * dist / MAXDIST)

#define POINTERUP(surface) p = (Uint8*) (surface->pixels) + 3 * (src.y * TEXSIZE + src.x)
#define DRAWPOINT(y) winpixels[y * WIDTH + x] =\
                          SDL_MapRGBA(surface->format, *p*v, *(p+1)*v, *(p+2)*v, 0xff)
void renderfloors(
	SDL_Surface* surface,
	Vec2d<float> pos,
	Vec2d<float> fieldleft, Vec2d<float> fieldright,
	SDL_Surface* floortex, SDL_Surface* ceiltex
)
{
	SDL_LockSurface(surface);
	Uint32* winpixels = (Uint32*) surface->pixels;
	SDL_Point src;
	Uint8* p;
	for (int y = HEIGHT / 2; y < HEIGHT; y++)
	{
		// distance to floor horizontally
		float dist = HEIGHT / (SQRT_2 * (y - HEIGHT / 2));
		Vec2d<float> step = {
			dist / WIDTH * (fieldright.x - fieldleft.x),
			dist / WIDTH * (fieldright.y - fieldleft.y),
			-1
		};
		// get map coords
		Vec2d<float> mappos = {
			pos.x + dist * fieldleft.x,
			pos.y + dist * fieldleft.y,
			-1
		};
		float v = FOGMOD(dist) / 255.f;
		for (int x = 0; x < WIDTH; x++)
		{
			src.x = (int) ((mappos.x - (int) mappos.x) * TEXSIZE) & TEXSIZE - 1;
			src.y = (int) ((mappos.y - (int) mappos.y) * TEXSIZE) & TEXSIZE - 1;
			POINTERUP(floortex); DRAWPOINT(y);
			POINTERUP(ceiltex);  DRAWPOINT((HEIGHT - y));

			mappos.x += step.x; mappos.y += step.y;
		}
	}
	SDL_UnlockSurface(surface);
}

void renderwalls(
	SDL_Surface* surface,
	Vec2d<float> pos, float heading,
	bool editmode,
	Map* map,
	Vec2d<float> vel /*fieldleft*/, Vec2d<float> fieldright,
	Vec2d<int> hl,
	SDL_Surface* textures[]
)
{
	// linear stepping since monitor isn't spherical
	Vec2d<float> step = {
		(fieldright.x - vel.x) / WIDTH,
		(fieldright.y - vel.y) / WIDTH,
		1
	};
	SDL_Rect src = { 0, 0, 1, TEXSIZE };
	for (int i = 0; i < WIDTH; i++)
	{
		vel.x += step.x; vel.y += step.y;
		Vec2d<int> tile;
		Vec2d<float> d = raycast(pos, vel, map, tile);
		// use cosine to get distance perpendicular to viewing plane
		d.mag *= cos(heading - atan2(vel.y, vel.x));
		// height to make the column
		float h = HEIGHT / d.mag; h *= (h >= 0);
		SDL_Rect r = { i, (int) ((HEIGHT - h) / 2), 1, (int) h };
		// if it is so close that the column is bigger than the screen,
		// only sample a portion of the texture and paste to full height of screen
		if (r.h > HEIGHT)
		{
			// trim proportional to screen clipping area
			src.y = -r.y * TEXSIZE / (float) r.h;
			src.h = TEXSIZE - 2 * src.y;
			if (src.h < 1) { src.y = TEXSIZE / 2; src.h = 1; }
			r.y = 0; r.h = HEIGHT;
		}
		else { src.y = 0; src.h = TEXSIZE; }
		// pick the texture, implement fog, and highlight it if needed
		SDL_Surface* t = textures[face_at(wall_at(map, tile.x, tile.y), tile.mag) - 1];
		Uint8 v = FOGMOD(d.mag);
		SDL_SetSurfaceColorMod(t, v, v * !(editmode && tile == hl), v);
		// sample at the right x location of the texture
		src.x = ((tile.mag % 2) ?  pos.x + d.x - tile.x : pos.y + d.y - tile.y) * TEXSIZE;
		// scale and paste the slice on the screen
		SDL_BlitScaled(t, &src, surface, &r);
	}
}

