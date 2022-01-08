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
int onclick(TTF_Font* font, GUIPage* page, SDL_Point mouse);
// returns 0 normally, 1 if the input should still be processed, and -1 if the page should close
int onkeypress(TTF_Font* font, GUIPage *page, SDL_Keycode key);
// returns 0 normally, 1 if the text was ignored because of overflow or no focus
int oninput(TTF_Font* font, GUIPage *page, const char* text);

#endif
