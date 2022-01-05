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

#define SET_COLORS(thing) thing.border = border; thing.bg = bg; thing.fg = fg

GUIThing button(
	TTF_Font* font,
	const char* label,
	int marginx, int marginy,
	COLOR_ARGS(,,)
)
{
	GUIThing g; g.type = GUI_BUTTON; SET_COLORS(g);
	SDL_Surface* text = TTF_RenderText_Shaded(font, label, fg, bg);
	SDL_Rect r = { marginx, marginy, text->w, text->h };
	g.s = SDL_CreateRGBSurface(
		0,
		r.w + r.x * 2, r.h + r.y * 2,
		32,
		0, 0, 0, 0xff
	);
	borderfill(g.s, border, bg);
	SDL_BlitSurface(text, NULL, g.s, &r);
	SDL_FreeSurface(text);
	g.r = { 0, 0, g.s->w, g.s->h };
	return g;
}

#define TEXTMARGIN 5
GUIThing inputbox(
	TTF_Font* font,
	const char* label,
	int w,
	SDL_Color outer,
	COLOR_ARGS(,,)
)
{
	GUIThing g; g.type = GUI_INPUT; SET_COLORS(g);
	int h = TTF_FontHeight(font) + 2;
	SDL_Surface* text = TTF_RenderText_Shaded(font, label, fg, outer);
	g.s = SDL_CreateRGBSurface(
		0,
		w + text->w + TEXTMARGIN, (h < text->h) ? text->h : h,
		32,
		0, 0, 0, 0xff
	);
	SDL_FillRect(g.s, NULL, SDL_MapRGBA(g.s->format, outer.r, outer.g, outer.b, outer.a));
	SDL_Rect r = { 0, (g.s->h - text->h) / 2, text->w, text->h };
	SDL_BlitSurface(text, NULL, g.s, &r);
	SDL_FreeSurface(text);
	g.textarea = { text->w + TEXTMARGIN, (g.s->h - h) / 2, w, h };
	SDL_FillRect(
		g.s, &g.textarea,
		SDL_MapRGBA(g.s->format, border.r, border.g, border.b, border.a)
	);
	g.textarea.x += 1; g.textarea.y += 1;
	g.textarea.w -= 2; g.textarea.h -= 2;
	SDL_FillRect(
		g.s, &g.textarea,
		SDL_MapRGBA(g.s->format, bg.r, bg.g, bg.b, bg.a)
	);
	g.r = { 0, 0, g.s->w, g.s->h };
	return g;
}

int redrawinput(TTF_Font* font, GUIThing& box, bool editing)
{
	int w, h;
	TTF_SizeText(font, box.value.c_str(), &w, &h);
	int overflown = w > box.textarea.w;
	if (overflown && box.value.size()) box.value.pop_back();
	// turn red when overflowing and blue while editing
	SDL_Color bg = box.bg;
	bg.r -= (editing && !overflown) * 0x10;
	bg.g -= editing * 0x10;
	bg.b -= overflown * 0x10;
	// draw textbox background and text
	SDL_FillRect(box.s, &box.textarea, SDL_MapRGBA(box.s->format, bg.r, bg.g, bg.b, bg.a));
	SDL_Rect r = box.textarea;
	if (box.value.size())
	{
		SDL_Surface* t = TTF_RenderText_Shaded(font, box.value.c_str(), box.fg, bg);
		SDL_BlitSurface(t, NULL, box.s, &r);
		SDL_FreeSurface(t);
	}
	// draw cursor
	if (!editing || overflown) return overflown;
	SDL_Rect cursor = { box.textarea.x + w + !w, box.textarea.y, 1, box.textarea.h };
	SDL_FillRect(
		box.s,
		&cursor,
		SDL_MapRGBA(box.s->format, box.fg.r, box.fg.g, box.fg.b, box.fg.a)
	);
	return overflown;
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
	GUIThing b, t; b.type = GUI_BACKDROP; SET_COLORS(b);
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
