#ifndef MAINGUI
#define MAINGUI
#include "utils.h"
#include SDL2_TTF_H
#include <string>
#include <vector>
#include "gui.h"
#include "guipage.h"
#include "map.h"
#include "rays.h"

struct maingui_data {
	GUIThing* namebox;
	GUIThing* savebut;
	GUIThing* loadbut;
	GUIThing* wallbut;
	std::string* mapname;
	Map* map;
};

// show available maps, returns new backdrop
GUIThing mapcolumns(std::vector<GUIThing> &guithings, TTF_Font* font, maingui_data &ptrs);

int  mgui_click(TTF_Font* font, GUIPage* page, GUIThing* thing);
void setup_mgui(GUIPage &mgui, TTF_Font* font, std::string &mapname, Map* map);

struct wallgui_data {
	GUIThing* start;
	Map* map;
	Vec2d<float>* pos;
	Vec2d<float>* fieldcenter;
	bool* editmode;
	Vec2d<int>* hl;
	int last_hl = 0;
};

void wallbutton_update(GUIThing* g);
void wallbutton_update(GUIThing* g, int overflown);
int  wallgui_click(TTF_Font* font, GUIPage* page, GUIThing* thing);
void wallgui_update(TTF_Font* font, GUIPage* page, int dt);
void wallgui_draw(SDL_Surface* surface, GUIPage* page);
void setup_wallgui(
	GUIPage &wallgui, TTF_Font* font, Map* map,
	Vec2d<float>* pos, Vec2d<float>* fieldcenter, bool* editmode, Vec2d<int>* hl
);

#endif
