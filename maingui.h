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

enum GameMode { GM_TITLESCREEN, GM_EDITING, GM_PLAYING };

// columns of available maps
void mapcolumns(std::vector<GUIThing> &guithings, TTF_Font* font, int numcols, SDL_Rect ref);

/* ---------- title gui (0) ---------- */
struct titlegui_data {
	int* gamemode;
	Map* map;
};
void titlegui_update(TTF_Font* font, GUIPage* page, int dt);
int titlegui_click(TTF_Font* font, GUIPage* page, GUIThing* thing);
void setup_titlegui(
	GUIPage &titlegui,
	TTF_Font* font, int draw_w, int draw_h,
	int* gamemode, Map* map
);

/* ---------- map selector gui (1) ---------- */
struct mapselgui_data {
	int* draw_w;
	int* draw_h;
	int* gamemode;
	Map* map;
};
int mapselgui_click(TTF_Font* font, GUIPage* page, GUIThing* thing);
int mapselgui_close(GUIPage* page, std::vector<GUIPage*> &history);
void setup_mapselgui(
	GUIPage &page, TTF_Font* font, int &draw_w, int &draw_h,
	int* gamemode, Map* map
);

/* ---------- edit gui (2) ---------- */
struct editgui_data {
	Map* map;
};
GUIThing editgui_listmaps(std::vector<GUIThing> &guithings, TTF_Font* font, std::string& mapname);
int  editgui_click(TTF_Font* font, GUIPage* page, GUIThing* thing);
void setup_editgui(
	GUIPage &editgui,
	TTF_Font* font, int draw_w, int draw_h,
	Map* map
);

/* ---------- wall gui (3) ---------- */
struct wallgui_data {
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
	GUIPage &wallgui, TTF_Font* font, int draw_w, int draw_h, Map* map,
	Vec2d<float>* pos, Vec2d<float>* fieldcenter, bool* editmode, Vec2d<int>* hl
);

#endif
