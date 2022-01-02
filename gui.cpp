#include "gui.h"
#include SDL2_H
#include SDL2_TTF_H
#include <vector>

void borderfill(SDL_Surface* surface, SDL_Color border, SDL_Color bg)
{
	Uint32* pixels = (Uint32*) surface->pixels;
	Uint32 b = SDL_MapRGBA(surface->format, border.r, border.g, border.b, border.a),
	       f = SDL_MapRGBA(surface->format, bg.r, bg.g, bg.b, bg.a);
	// draw border and background
	for (int i = 0, x, y; i < surface->w * surface->h; i++)
	{
		x = i % surface-> w; y = i / surface-> w;
		pixels[i] = (x % (surface->w - 1) && y % (surface->h - 1)) ? f : b;
	}
}

SDL_Surface* button(
	TTF_Font* font,
	const char* label,
	int marginx, int marginy,
	COLOR_ARGS(,,)
)
{
	SDL_Surface* text = TTF_RenderText_Shaded(font, label, fg, bg);
	SDL_Rect r = { marginx, marginy, text->w, text->h };
	SDL_Surface* surface = SDL_CreateRGBSurface(
		0,
		r.w + r.x * 2, r.h + r.y * 2,
		32,
		0, 0, 0, 0xff
	);
	borderfill(surface, border, bg);
	SDL_BlitSurface(text, NULL, surface, &r);
	SDL_FreeSurface(text);
	return surface;
}

// sets b to a if a comp b is true
#define SETB_IF(a, comp, b) if (a comp b) b = a
GUIThing backdrop(
	std::vector<GUIThing> guithings,
	TTF_Font* font,
	const char* title,
	int marginx, int marginy,
	COLOR_ARGS(,,)
)
{
	GUIThing b;
	GUIThing t;
	if (title) t.s = TTF_RenderText_Shaded(font, title, fg, bg);
	if (!guithings.size())
	{
		if (!title) return b;
		b.r = { 0, 0, t.s->w, t.s->h };
	}
	else b.r = guithings[0].r;
	// using b.r w and h as bottom left coordinates, find bounds
	for (int i = 1; i < guithings.size(); i++)
	{
		SDL_Rect g = guithings[i].r;
		// expand to include g in b.r
		SETB_IF(g.x,       <, b.r.x); SETB_IF(g.y,       <, b.r.y);
		SETB_IF(g.x + g.w, >, b.r.w); SETB_IF(g.y + g.h, >, b.r.h);
	}
	// make b.r w and h actual width and height values
	b.r.w -= b.r.x; b.r.h -= b.r.y;
	// apply margins
	b.r.x -= marginx; b.r.w += marginx * 2;
	b.r.y -= marginy; b.r.h += marginy * 2;
	if (title)
	{
		// include title if it wasn't already
		if (guithings.size()) { b.r.y -= t.s->h + marginy; b.r.h += t.s->h + marginy; }
		t.r.w = t.s->w; t.r.h = t.s->h;
		// middle top
		t.r.x = (b.r.w - t.r.w) / 2;
		t.r.y = marginy;
	}
	// render this
	b.s = SDL_CreateRGBSurface(0, b.r.w, b.r.h, 32, 0, 0, 0, 0xff);
	borderfill(b.s, border, bg);
	if (title)
	{
		SDL_BlitSurface(t.s, NULL, b.s, &t.r);
		SDL_FreeSurface(t.s);
	}
	return b;
}
