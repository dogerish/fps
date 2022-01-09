#ifndef MAINGUI
#define MAINGUI
#include "utils.h"
#include SDL2_TTF_H
#include <string>
#include <vector>
#include "gui.h"
#include "guipage.h"
#include "map.h"

struct maingui_data {
	GUIThing* namebox;
	GUIThing* savebut;
	GUIThing* loadbut;
	std::string* mapname;
	Map* map;
};

// show available maps, returns new backdrop
GUIThing mapcolumns(std::vector<GUIThing> &guithings, TTF_Font* font, maingui_data &ptrs);

void mgui_click(TTF_Font* font, GUIPage* page, GUIThing* thing);

#endif
