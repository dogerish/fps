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
	// return 0 normally, or 1 + index of gui to open, or -1 to close
	int (*button_click)(TTF_Font* font, GUIPage* page, GUIThing* thing);
	// return non-zero if the page shouldn't close
	int (*page_close)(GUIPage* page, std::vector<GUIPage*> &history) = NULL;
	void (*update)(TTF_Font* font, GUIPage* page, int dt) = NULL;
	void (*draw)(SDL_Surface* surface, GUIPage* page) = NULL;
};

GUIThing* startinput(TTF_Font* font, GUIThing* box);
GUIThing* stopinput(TTF_Font* font, GUIThing* focused);

/* return code:
	-2 - i: close this page and then open gui index <i>. this page won't be restored.
	-1    : close this page
	 0    : do nothing special (normal return)
	 1    : undefined
	 2 + i: open gui index <i>, restore this page when that gui is closed
*/
int onclick(TTF_Font* font, GUIPage* page, SDL_Point mouse);
// returns 0 normally, 1 if the input should still be processed, and -1 if the page should close
int onkeypress(TTF_Font* font, GUIPage *page, SDL_Keycode key);
// returns 0 normally, 1 if the text was ignored because of overflow or no focus
int oninput(TTF_Font* font, GUIPage *page, const char* text);

void default_pagedraw(SDL_Surface* surface, GUIPage* page);
void drawgui(SDL_Surface* surface, GUIPage *page, TTF_Font* font, int dt);

// returns -1 if the page wasn't closed. normal return 0
int  closegui(GUIPage* &current, std::vector<GUIPage*> &history);
void opengui(GUIPage* &current, GUIPage* page, std::vector<GUIPage*> &history);

// centers the page and its contents around center
void center_page(GUIPage &page, SDL_Point center);

#endif
