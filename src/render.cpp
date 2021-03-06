#include "render.h"
#include <cmath>
#include "utils.h"
#include "vec.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include "rays.h"
#include "map.h"

#define MAXDIST 30.f
// value to use for color mod to give fog
#define FOGMOD(dist) (0xff - 0xef * dist / MAXDIST)

void drawfps(SDL_Surface* surface, TTF_Font* font, float fps)
{
	char infostr[6 + (int) fps / 10];
	// render info text
	sprintf(infostr, "%.1f fps", fps);
	int orig = TTF_GetFontStyle(font);
	TTF_SetFontStyle(font, TTF_STYLE_BOLD);
	SDL_Surface* text = TTF_RenderText_Solid(font, infostr, { 255, 255, 255, 255 });
	TTF_SetFontStyle(font, orig);
	SDL_Rect r = { surface->w - text->w - 5, surface->h - text->h - 5, text->w, text->h };
	SDL_BlitSurface(text, NULL, surface, &r);
	SDL_FreeSurface(text);
}

void renderfloors(
	SDL_Surface* surface,
	Vec2d<float> pos,
	Vec2d<float> fieldleft, Vec2d<float> fieldright,
	SDL_Surface* floortex, SDL_Surface* ceiltex
)
{
	int draw_w = surface->w, draw_h = surface->h;
	SDL_LockSurface(surface);
	Uint32* winpixels = (Uint32*) surface->pixels;
	Uint32* f = (Uint32*) floortex->pixels, *c = (Uint32*) ceiltex->pixels;
	float dist, stx, sty, mpx, mpy;
	int sx, sy;
	Uint32 color;
	Uint8 v;
	for (int y = draw_h / 2; y < draw_h; y++)
	{
		// distance to floor horizontally
		dist = draw_h / (SQRT_2 * (y - draw_h / 2));
		stx = dist / draw_w * (fieldright.x - fieldleft.x);
		sty = dist / draw_w * (fieldright.y - fieldleft.y);
		// get map coords
		mpx = pos.x + dist * fieldleft.x;
		mpy = pos.y + dist * fieldleft.y;
		v = FOGMOD(dist);
		for (int x = 0; x < draw_w; x++)
		{
			sx = (int) ((mpx - (int) mpx) * TEXSIZE) & TEXSIZE - 1;
			sy = (int) ((mpy - (int) mpy) * TEXSIZE) & TEXSIZE - 1;
			color = c[sy * ceiltex->w + sx];
			winpixels[(draw_h - 1 - y) * draw_w + x] =
				(color >> 16 & 0xff) * v / 255 << 16 |
				(color >>  8 & 0xff) * v / 255 <<  8 |
				(color       & 0xff) * v / 255;
			color = f[sy * floortex->w + sx];
			winpixels[y * draw_w + x] =
				(color >> 16 & 0xff) * v / 255 << 16 |
				(color >>  8 & 0xff) * v / 255 <<  8 |
				(color       & 0xff) * v / 255;

			mpx += stx; mpy += sty;
		}
	}
	SDL_UnlockSurface(surface);
}

void renderwalls(
	SDL_Surface* surface,
	Vec2d<float> pos,
	bool editmode,
	Map* map,
	Vec2d<float> vel /*fieldleft*/, Vec2d<float> fieldright,
	Vec2d<int> hl,
	SDL_Surface* textures[]
)
{
	int draw_w = surface->w, draw_h = surface->h;
	// linear stepping since monitor isn't spherical
	Vec2d<float> step = {
		(fieldright.x - vel.x) / draw_w,
		(fieldright.y - vel.y) / draw_w,
		1
	};
	SDL_Rect src = { 0, 0, 1, TEXSIZE };
	for (int i = 0; i < draw_w; i++)
	{
		vel.x += step.x; vel.y += step.y;
		Vec2d<int> tile;
		Vec2d<float> d = raycast(pos, vel, map, tile);
		// use cosine to get distance perpendicular to viewing plane
		d.mag *= cos(pos.mag - atan2(vel.y, vel.x));
		// height to make the column
		float h = draw_h / d.mag; h *= (h >= 0);
		SDL_Rect r = { i, (int) ((draw_h - h) / 2), 1, (int) h };
		// if it is so close that the column is bigger than the screen,
		// only sample a portion of the texture and paste to full height of screen
		if (r.h > draw_h)
		{
			// trim proportional to screen clipping area
			src.y = -r.y * TEXSIZE / (float) r.h;
			src.h = TEXSIZE - 2 * src.y;
			if (src.h < 1) { src.y = TEXSIZE / 2; src.h = 1; }
			r.y = 0; r.h = draw_h;
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

