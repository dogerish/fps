#ifndef RCGUI
#define RCGUI
#include "utils.h"
#include SDL2_H
#include SDL2_TTF_H
#include <vector>
#include <string>

// SDL_Color initialization lists for common colors
#define BLACK(alpha)  { 0x00, 0x00, 0x00, alpha }
#define DGRAY(alpha)  { 0x40, 0x40, 0x40, alpha }
#define  GRAY(alpha)  { 0x80, 0x80, 0x80, alpha }
#define LGRAY(alpha)  { 0xc0, 0xc0, 0xc0, alpha }
#define WHITE(alpha)  { 0xff, 0xff, 0xff, alpha }
#define RED(alpha)    { 0xff, 0x00, 0x00, alpha }
#define GREEN(alpha)  { 0x00, 0xff, 0x00, alpha }
#define BLUE(alpha)   { 0x00, 0xff, 0x00, alpha }

#define COLOR_ARGS(dborder, dbg, dfg) \
	SDL_Color border dborder,\
	SDL_Color bg dbg,\
	SDL_Color fg dfg
#define DEFAULT_COLOR_ARGS COLOR_ARGS(= DGRAY(0xff),= LGRAY(0xff),= BLACK(0xff))

enum GUIType {
	GUI_BUTTON,
	GUI_INPUT,
	GUI_BACKDROP
};

struct GUIThing {
	int type;
	SDL_Rect r;
	SDL_Rect textarea;
	SDL_Surface* s;
	std::string value;
	SDL_Color border;
	SDL_Color bg;
	SDL_Color fg;
};

void borderfill(SDL_Surface* surface, SDL_Color border, SDL_Color bg);

GUIThing button(
	TTF_Font* font,
	const char* label,
	int marginx = 5, int marginy = 1,
	DEFAULT_COLOR_ARGS
);

GUIThing inputbox(
	TTF_Font* font,
	const char* label,
	int w,
	SDL_Color outer,
	DEFAULT_COLOR_ARGS
);

// returns 1 if the text overflowed
int redrawinput(TTF_Font* font, GUIThing& box, bool editing = true);

// set title to NULL for no title
GUIThing backdrop(
	std::vector<GUIThing> guithings,
	TTF_Font* font,
	const char* title,
	int marginx = 5, int marginy = 5,
	COLOR_ARGS(= BLACK(0xff),= GRAY(0xff),= BLACK(0xff))
);

#endif
