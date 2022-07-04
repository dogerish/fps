#ifndef MAINGUI
#define MAINGUI
#include <SDL_ttf.h>
#include <string>
#include <vector>
#include "gui.h"
#include "guipage.h"
#include "map.h"
#include "rays.h"
#include "game.h"

#define SETUP_ARGS GUIPage &page, TTF_Font* font, GameData* gd, int &draw_w, int &draw_h
#define PASS_SETUP_ARGS(i) guipages[i], font, &gd, draw_w, draw_h

// columns of available maps
void mapcolumns(
	std::vector<GUIThing> &guithings,
	TTF_Font* font, int numcols, SDL_Rect ref,
	const std::string &resource_path
);

/* ---------- title gui (0) ---------- */
void titlegui_update(GUIPage* page, int dt);
int titlegui_click(GUIPage* page, GUIThing* thing);
void setup_titlegui(SETUP_ARGS);

/* ---------- map selector gui (1) ---------- */
struct mapselgui_data {
	int* draw_w;
	int* draw_h;
};
void mapselgui_refresh(GUIPage &page);
int mapselgui_click(GUIPage* page, GUIThing* thing);
int mapselgui_close(GUIPage* page, std::vector<GUIPage*> &history);
void setup_mapselgui(SETUP_ARGS);

/* ---------- edit gui (2) ---------- */
GUIThing editgui_listmaps(
	std::vector<GUIThing> &guithings, TTF_Font* font, std::string& mapname,
	const std::string &resource_path
);
int editgui_click(GUIPage* page, GUIThing* thing);
void editgui_update(GUIPage* page, int dt);
void setup_editgui(SETUP_ARGS);

/* ---------- wall gui (3) ---------- */
struct wallgui_data {
	int last_hl = 0;
};
void wallbutton_update(GUIThing* g);
void wallbutton_update(GUIThing* g, int overflown);
int wallgui_click(GUIPage* page, GUIThing* thing);
void wallgui_update(GUIPage* page, int dt);
void wallgui_draw(SDL_Surface* surface, GUIPage* page);
void setup_wallgui(SETUP_ARGS);

#endif
