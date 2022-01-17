#include "maingui.h"
#include "utils.h"
#include SDL2_TTF_H
#include <string>
#include <vector>
#include <filesystem>
#include "gui.h"
#include "guipage.h"
#include "map.h"
namespace fs = std::filesystem;

// indices for important maingui things
enum MAINGUI_INDICES { MGN, MGS, MGL, MGW, MGNEW };

// show available maps, returns new backdrop
GUIThing mapcolumns(std::vector<GUIThing> &guithings, TTF_Font* font, std::string& mapname)
{
	std::vector<std::string> entries;
	for (auto entry : fs::directory_iterator("maps/"))
		entries.push_back(entry.path().filename().string());
	SDL_Rect href = guithings.back().r;
	columnate(
		guithings, font, "Maps", entries,
		{ guithings[MGN].r.x, href.y, guithings[MGN].r.w, href.h },
		5, 5,
		GRAY(0xff), DGRAY(0xff), GRAY(0xff)
	);
	return backdrop(guithings, font, mapname.c_str());
}

int mgui_click(TTF_Font* font, GUIPage* page, GUIThing* thing)
{
	maingui_data* data = (maingui_data*) page->userdata;
	int idx = thing - &page->things[0];
	switch (idx)
	{
	case MGW: return 2; // open wall gui
	case MGS: // save map
		if (savemap(data->map, page->things[MGN].value)) return 0;
		// update map listing
		for (int i = page->things.size() - 1; page->things[i].type != GUI_TEXT; i--)
		{
			SDL_FreeSurface(page->things[i].s);
			page->things.pop_back();
		}
		SDL_FreeSurface(page->things.back().s);
		page->things.pop_back();
		SDL_FreeSurface(page->bdr.s);
		page->bdr = mapcolumns(page->things, font, *data->mapname);
		break;
	case MGL: // load map or new map
		if (loadmap(data->map, page->things[MGN].value)) return 0;
	case MGNEW:
		*data->mapname = (idx == MGNEW) ? "newmap" : page->things[MGN].value;
		if (idx == MGNEW) newmap(data->map);
		SDL_FreeSurface(page->bdr.s);
		page->bdr = backdrop(
			page->things,
			font, data->mapname->c_str(),
			5, 5, page->bdr.border, page->bdr.bg, page->bdr.fg
		);
		break;
	default: // select map
		page->things[MGN].value = thing->value;
		redrawinput(font, &page->things[MGN], false);
		break;
	}
	return 0;
}

void setup_mgui(
	GUIPage &mgui,
	TTF_Font* font, int draw_w, int draw_h,
	std::string &mapname, Map* map
)
{
	maingui_data* ptrs = new maingui_data;
	mgui.userdata = ptrs;
	mgui.button_click = mgui_click;
	// reserve enough memory so that the pointers remain valid
	mgui.things.reserve(5);
	// add things and ste up pointers
	GUIThing* ref = addthing(mgui.things, inputbox(font, "Map name:", 100));
	addthing(mgui.things, button(font, "Save"), ALIGN_RIGHT);
	addthing(mgui.things, button(font, "Load"), ALIGN_LEFT, &ref->r);
	ref = addthing(mgui.things, button(font, "Wall Editor"), ALIGN_CENTER, &ref->r);
	addthing(mgui.things, button(font, "New Map"), ALIGN_CENTER, &ref->r);
	ptrs->mapname = &mapname;
	ptrs->map     = map;
	mgui.bdr      = mapcolumns(mgui.things, font, mapname);
	center_page(mgui, { draw_w / 2, draw_h / 2 });
}

// starting index of the wall buttons
#define WGS 0

void wallbutton_update(GUIThing* g)
{
	Uint8 v = 0xff - g->overflown * 0x80;
	SDL_SetSurfaceColorMod(g->s, v, v, v);
}
void wallbutton_update(GUIThing* g, int overflown)
{
	g->overflown = overflown;
	wallbutton_update(g);
}

int wallgui_click(TTF_Font* font, GUIPage* page, GUIThing* thing)
{
	wallgui_data* data = (wallgui_data*) page->userdata;
	int i = thing - &page->things[WGS];
	// refresh buttons
	if (thing == &page->things.back())
	{
		for (int j = 0; j < data->map->w * data->map->h; j++)
			wallbutton_update(&page->things[WGS + j], data->map->data[j].clip);
		return 0;
	}
	data->map->data[i].clip = thing->overflown = !thing->overflown;
	if (data->map->data[i].n == 0) set_faces(&data->map->data[i], 1, 1, 1, 1);
	wallbutton_update(thing);
	return 0;
}

void wallgui_update(TTF_Font* font, GUIPage* page, int dt)
{
	wallgui_data* data = (wallgui_data*) page->userdata;
	// highlight tile that is being looked at
	int new_hl = data->hl->x + data->hl->y * data->map->w;
	wallbutton_update(&page->things[WGS + data->last_hl]);
	if (*data->editmode && withinmap(data->map, data->hl->x, data->hl->y))
	{
		data->last_hl = new_hl;
		SDL_SetSurfaceColorMod(page->things[WGS + data->last_hl].s, 0xff, 0, 0xff);
	}
}
#define SQRT2OVER4 0.3535533906 // sqrt(2) / 4
void wallgui_draw(SDL_Surface* surface, GUIPage* page)
{
	default_pagedraw(surface, page);
	// draw player
	wallgui_data* data = (wallgui_data*) page->userdata;
	Uint32 color = SDL_MapRGBA(surface->format, 0xd0, 0, 0xff, 0xd0);
	SDL_Rect r = { (int) (data->pos->x * 10), (int) (data->pos->y * 10), 7, 7 };
	r.x += page->things[WGS].r.x;
	r.y += page->things[WGS].r.y;
	int len = 10, stroke = 3;
	// if facing > 112.5 deg: -1; < 22.5 deg: 1; 0; but don't care about top or bottom hemi
	// above, but using unit circle magic
	int ix = data->fieldcenter->x / SQRT2OVER4;
	if (ix > 1 || ix < -1) ix = (ix < 0) ? -1 : 1;
	int iy = data->fieldcenter->y / SQRT2OVER4;
	if (iy > 1 || iy < -1) iy = (iy < 0) ? -1 : 1;
	// straight line optimization
	if (!ix || !iy)
	{
		SDL_Rect l = {
			r.x - !ix * stroke / 2, r.y - !iy * stroke / 2,
			ix * len + !ix * stroke, iy * len + !iy * stroke
		};
		if (ix < 0) l.x -= (l.w *= -1);
		if (iy < 0) l.y -= (l.h *= -1);
		SDL_FillRect(surface, &l, color);
	}
	// diagonal line
	else
	{
		SDL_Rect rects[(len *= 0.7071067812)];
		for (int i = 0; i < len; i++)
			rects[i] = { r.x + i * ix - stroke / 2, r.y + i * iy - stroke / 2, stroke, stroke };
		SDL_FillRects(surface, rects, len, color);
	}
	// finish drawing player dot
	r.x -= r.w / 2; r.y -= r.w / 2;
	SDL_FillRect(surface, &r, color);
}

void setup_wallgui(
	GUIPage &wallgui, TTF_Font* font, int draw_w, int draw_h, Map* map,
	Vec2d<float>* pos, Vec2d<float>* fieldcenter, bool* editmode, Vec2d<int>* hl
)
{
	wallgui_data* ptrs = new wallgui_data;
	wallgui.userdata = ptrs;
	wallgui.button_click = wallgui_click;
	wallgui.update = wallgui_update;
	wallgui.draw = wallgui_draw;
	wallgui.things.reserve(map->w * map->h + 1);
	for (int y = 0; y < map->h; y++)
	{
		for (int x = 0; x < map->w; x++)
		{
			GUIThing* g;
			if (!x && y) g = addthing(
				wallgui.things, button(10, 10, GRAY(0xff)),
				ALIGN_LEFT, &wallgui.things[(y - 1) * map->w].r, 0
			);
			else g = addthing(
				wallgui.things, button(10, 10, GRAY(0xff)),
				ALIGN_TOP, NULL, 0
			);
			wallbutton_update(g, wall_at(map, x, y)->clip);
		}
	}
	addthing(wallgui.things, button(font, "Refresh"), ALIGN_RIGHT);
	ptrs->map = map;
	ptrs->pos = pos; ptrs->fieldcenter = fieldcenter; ptrs->editmode = editmode; ptrs->hl = hl;
	wallgui.bdr = backdrop(wallgui.things, font, "Wall Editor", 10, 10);
	center_page(wallgui, { draw_w / 2, draw_h / 2 });
}
