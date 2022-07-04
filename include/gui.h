#ifndef RCGUI
#define RCGUI
#include <SDL.h>
#include <SDL_ttf.h>
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
#define BLUE(alpha)   { 0x00, 0x00, 0xff, alpha }

#define COLOR_ARGS(dborder, dbg, dfg) \
	SDL_Color border dborder,\
	SDL_Color bg dbg,\
	SDL_Color fg dfg
#define DEFAULT_COLOR_ARGS COLOR_ARGS(= DGRAY(0xff),= LGRAY(0xff),= BLACK(0xff))

enum GUIType {
	GUI_TEXT,
	GUI_BUTTON,
	GUI_INPUT,
	GUI_BACKDROP
};
enum GUISubType {
	GUIST_DEFAULT,
	GUIST_NUMINPUT
};

struct GUIThing {
	GUIType    type;
	GUISubType subtype = GUIST_DEFAULT;
	int id = -1;
	SDL_Rect r;
	SDL_Rect textarea;
	SDL_Surface* s = NULL;
	std::string value;
	int overflown = 0;
	bool shown = true;
	SDL_Color border;
	SDL_Color bg;
	SDL_Color fg;
};

void borderfill(SDL_Surface* surface, SDL_Color border, SDL_Color bg);

GUIThing textbox(
	TTF_Font* font,
	const char* text,
	SDL_Color bg = GRAY(0xff),
	SDL_Color fg = WHITE(0xff)
);

GUIThing button(
	TTF_Font* font,
	const char* label,
	int marginx = 5, int marginy = 1,
	DEFAULT_COLOR_ARGS
);
GUIThing button(int w, int h, DEFAULT_COLOR_ARGS);

GUIThing inputbox(
	TTF_Font* font,
	const char* label,
	int w,
	SDL_Color outer = GRAY(0xff),
	DEFAULT_COLOR_ARGS
);
GUIThing numinputbox(
	TTF_Font* font,
	const char* label,
	int w,
	SDL_Color outer = GRAY(0xff),
	DEFAULT_COLOR_ARGS
);

void redrawinput(TTF_Font* font, GUIThing* box, bool editing = true);

// set title to NULL for no title
GUIThing backdrop(
	std::vector<GUIThing> &guithings,
	TTF_Font* font,
	const char* title,
	int marginx = 5, int marginy = 5,
	COLOR_ARGS(= BLACK(0xff),= GRAY(0xff),= WHITE(0xff))
);

enum ThingAlignment {
	/************************************************/ ALIGN_TOP = 0,
	/************* Reference Rectangle **************/ ALIGN_MIDDLE = 1,
	/************************************************/ ALIGN_BOTTOM = 2,
	ALIGN_LEFT = 3, ALIGN_CENTER = 4, ALIGN_RIGHT = 5,
};
// returns a pointer to the GUIThing that was added
GUIThing* addthing(
	std::vector<GUIThing> &guithings,
	GUIThing thing,
	ThingAlignment align = ALIGN_CENTER,
	SDL_Rect* ref = NULL,
	int margin = 5
);

// make column layout for list of strings which will be made into buttons which are appended
// NULL title for none
void columnate(
	std::vector<GUIThing> &guithings,
	TTF_Font* font,
	const char* title,
	std::vector<std::string> &strings, unsigned int numcols,
	SDL_Rect alignto,
	int marginx = 5, int marginy = 5,
	SDL_Color outer = GRAY(0xff),
	DEFAULT_COLOR_ARGS
);
#endif
