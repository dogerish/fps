#ifndef GUIPAGE
#define GUIPAGE
#include "utils.h"
#include SDL2_H
#include SDL2_TTF_H
#include "gui.h"
#include <vector>

struct GUIPage {
	int id;
	GUIThing bdr;
	std::vector<GUIThing> things;
	GUIThing* focused = NULL;
	void* userdata = NULL;
	void (*button_click)(TTF_Font* font, GUIPage* page, GUIThing* thing);
};

GUIThing* startinput(TTF_Font* font, GUIThing* box);
GUIThing* stopinput(TTF_Font* font, GUIThing* focused);

// returns 0 if something was clicked, 1 if it was the backdrop, and -1 if the page should close
int onclick(TTF_Font* font, GUIPage *page, SDL_Point mouse);

#endif
