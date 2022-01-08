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
	bool f = page->focused;
	if (f) page->focused = stopinput(font, page->focused);
	bool inbdr = SDL_PointInRect(&mouse, &page->bdr.r);
	int i = 0, limit = page->things.size();
	if (inbdr)
	{
		for (; i < limit; i++)
		{
			GUIThing* g = &page->things[i];
			if (!SDL_PointInRect(&mouse, &g->r)) continue;
			if      (g->type == GUI_INPUT)  page->focused = startinput(font, g);
			else if (g->type == GUI_BUTTON) page->button_click(font, page, g);
			else continue;
			break;
		}
	}
	// button clicked
	if (inbdr && i != limit || f) return 0;
	// no button clicked; 1 for backdrop, -1 if not
	return inbdr * 2 - 1;
}

int onkeypress(TTF_Font* font, GUIPage *page, SDL_Keycode key)
{
	if (!page->focused && key == SDLK_ESCAPE) return -1;
	else if (!page->focused) return 1;
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
