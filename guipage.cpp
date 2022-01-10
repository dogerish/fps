#include "guipage.h"
#include "utils.h"
#include SDL2_H
#include SDL2_TTF_H
#include "gui.h"

GUIThing* startinput(TTF_Font* font, GUIThing* box)
{
	SDL_StartTextInput();
	redrawinput(font, box);
	return box;
}
GUIThing* stopinput(TTF_Font* font, GUIThing* focused)
{
	SDL_StopTextInput();
	redrawinput(font, focused, false);
	return NULL;
}

int onclick(TTF_Font* font, GUIPage* page, SDL_Point mouse)
{
	if (page->focused) page->focused = stopinput(font, page->focused);
	// close if click happened outside of backdrop
	if (!SDL_PointInRect(&mouse, &page->bdr.r)) return -1;
	for (GUIThing &g : page->things)
	{
		if (!SDL_PointInRect(&mouse, &g.r)) continue;
		if      (g.type == GUI_INPUT)  page->focused = startinput(font, &g);
		else if (g.type == GUI_BUTTON) return page->button_click(font, page, &g);
		else continue;
		break;
	}
	return 0;
}

int onkeypress(TTF_Font* font, GUIPage *page, SDL_Keycode key)
{
	if (!page->focused) return 1 - (key == SDLK_ESCAPE) * 2;
	switch (key)
	{
	case SDLK_ESCAPE:
	case SDLK_RETURN: page->focused = stopinput(font, page->focused); break;
	case SDLK_BACKSPACE:
		if (!page->focused->value.size()) break;
		if (!page->focused->overflown) page->focused->value.pop_back();
		redrawinput(font, page->focused);
		break;
	}
	return 0;
}

int oninput(TTF_Font* font, GUIPage *page, const char* text)
{
	if (!page->focused || page->focused->overflown) return 1;
	page->focused->value += text;
	redrawinput(font, page->focused);
	return 0;
}

void default_pagedraw(SDL_Surface* surface, GUIPage* page)
{
	SDL_BlitSurface(page->bdr.s, NULL, surface, &page->bdr.r);
	for (GUIThing g : page->things) SDL_BlitSurface(g.s, NULL, surface, &g.r);
}
void drawgui(SDL_Surface* surface, GUIPage *page, TTF_Font* font, int dt)
{
	if (page->update) page->update(font, page, dt);
	if (page->draw) page->draw(surface, page);
	else default_pagedraw(surface, page);
}

void closegui(GUIPage* &current, std::vector<GUIPage*> &history)
{
	if (!history.size()) { current = NULL; return; }
	current = history.back();
	history.pop_back();
}
void opengui(GUIPage* &current, GUIPage* page, std::vector<GUIPage*> &history)
{
	if (current) history.push_back(current);
	current = page;
}

void center_page(GUIPage &page, SDL_Point center)
{
	// calculate the offset from the current center and move everything by that offset
	center.x -= page.bdr.r.x + page.bdr.r.w / 2;
	center.y -= page.bdr.r.y + page.bdr.r.h / 2;
	page.bdr.r.x += center.x;
	page.bdr.r.y += center.y;
	for (GUIThing &g : page.things) { g.r.x += center.x; g.r.y += center.y; }
}
