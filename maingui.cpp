#include "maingui.h"
#include "utils.h"
#include SDL2_TTF_H
#include <string>
#include <vector>
#include <filesystem>
#include "gui.h"
#include "guipage.h"
#include "map.h"
#include "game.h"
namespace fs = std::filesystem;

#define GETDATA(type) type* data = (type*) page->userdata

void mapcolumns(std::vector<GUIThing> &guithings, TTF_Font* font, int numcols, SDL_Rect ref)
{
	std::vector<std::string> entries;
	for (auto entry : fs::directory_iterator("maps/"))
		entries.push_back(entry.path().filename().string());
	columnate(
		guithings, font, "Maps", entries, numcols, ref,
		5, 5, GRAY(0xff), DGRAY(0xff), GRAY(0xff)
	);
}

enum TITLEGUI_INDICES { TGHOME, TGPLAY, TGEDIT, TGSELMAP, TGEDITMAP, TGSETTINGS, TGEXIT };
void titlegui_update(GUIPage* page, int dt)
{
	int gamemode = page->gd->gamemode;
	page->things[TGHOME].shown = gamemode != GM_TITLESCREEN;
	page->things[TGPLAY].shown = gamemode != GM_PLAYING;
	page->things[TGEDIT].shown = gamemode != GM_EDITING;
	page->things[TGSELMAP].shown  = gamemode != GM_EDITING && gamemode != GM_TITLESCREEN;
	page->things[TGEDITMAP].shown = gamemode == GM_EDITING;
}
int titlegui_click(GUIPage* page, GUIThing* thing)
{
	int i = thing - &page->things[0];
	switch (i)
	{
	case TGHOME: page->gd->gamemode = GM_TITLESCREEN; return 0;
	// play / edit
	case TGPLAY:
	case TGEDIT:
		page->gd->map->loaded = page->gd->gamemode != GM_TITLESCREEN;
		page->gd->gamemode = (i == TGPLAY) ? GM_PLAYING : GM_EDITING;
	case TGSELMAP:   return -2 - 1; // close page and select map
	case TGEDITMAP:  return  2 + 2; // edit gui
	case TGSETTINGS: return  2 + 4; // settings
	case TGEXIT:
		SDL_Event e; e.type = SDL_QUIT;
		SDL_PushEvent(&e);
	default: return 0;
	}
}
void setup_titlegui(SETUP_ARGS)
{
	page.font = font; page.gd = gd;
	page.update = titlegui_update;
	page.button_click = titlegui_click;
	page.things.reserve(5);
	SDL_Rect leftref = addthing(page.things, button(font, "Home"))->r;
	SDL_Rect ref = addthing(page.things, button(font, "Play"), ALIGN_TOP)->r;
	addthing(page.things, button(font, "Edit"), ALIGN_TOP);
	addthing(page.things, button(font, "Select Map"), ALIGN_CENTER, &ref);
	addthing(page.things, button(font, "Edit..."), ALIGN_CENTER, &ref);
	ref = addthing(page.things, button(font, "Settings"))->r;
	ref = { leftref.x, ref.y, leftref.w, ref.h };
	addthing(page.things, button(font, "Exit", 5, 1, RED(0xff)), ALIGN_LEFT, &ref, 15);
	page.bdr = backdrop(page.things, font, "Main Menu");
	center_page(page, { draw_w / 2, draw_h / 2 });
}

void mapselgui_refresh(GUIPage &page)
{
	for (GUIThing &g : page.things) if (g.id != 0) SDL_FreeSurface(g.s);
	if (page.things.size() > 1) page.things.erase(page.things.begin() + 1, page.things.end());
	mapcolumns(page.things, page.font, 5, page.things.back().r);
	SDL_Rect ref = page.things.back().r;
	if (page.bdr.s) SDL_FreeSurface(page.bdr.s);
	page.bdr = backdrop(page.things, page.font, page.bdr.value.c_str());
}
int mapselgui_click(GUIPage* page, GUIThing* thing)
{
	GETDATA(mapselgui_data);
	// select map
	if (thing->id < 0)
	{
		loadmap(page->gd->map, thing->value);
		return -1;
	}
	// refresh
	mapselgui_refresh(*page);
	center_page(*page, { *data->draw_w / 2, *data->draw_h / 2 });
	return 0;
}
int mapselgui_close(GUIPage* page, std::vector<GUIPage*> &history)
{
	if (!page->gd->map->loaded) page->gd->gamemode = GM_TITLESCREEN;
	return 0;
}
void setup_mapselgui(SETUP_ARGS)
{
	page.font = font; page.gd = gd;
	mapselgui_data* data = new mapselgui_data;
	page.userdata = data;
	data->draw_w = &draw_w;
	data->draw_h = &draw_h;
	page.button_click = mapselgui_click;
	page.page_close = mapselgui_close;
	GUIThing refresh_button = button(font, "Refresh");
	refresh_button.id = 0;
	addthing(page.things, refresh_button);
	page.bdr.value = "Map Selector";
	mapselgui_refresh(page);
	center_page(page, { draw_w / 2, draw_h / 2 });
}

// indices for important maingui things
enum EDITGUI_INDICES { EGNAME, EGSAVE, EGLOAD, EGWALL, EGNEWMAP };
// show available maps, returns new backdrop
GUIThing editgui_listmaps(std::vector<GUIThing> &guithings, TTF_Font* font, std::string& mapname)
{
	SDL_Rect ref = guithings.back().r;
	ref.x = guithings[EGNAME].r.x;
	ref.w = guithings[EGNAME].r.w;
	mapcolumns(guithings, font, 3, ref);
	return backdrop(guithings, font, mapname.c_str());
}
int editgui_click(GUIPage* page, GUIThing* thing)
{
	int idx = thing - &page->things[0];
	switch (idx)
	{
	case EGWALL: return 2 + 3; // open wall gui
	case EGSAVE: // save map
		if (savemap(page->gd->map, page->things[EGNAME].value)) return 0;
		// update map listing
		for (int i = page->things.size() - 1; page->things[i].type != GUI_TEXT; i--)
		{
			SDL_FreeSurface(page->things[i].s);
			page->things.pop_back();
		}
		SDL_FreeSurface(page->things.back().s);
		page->things.pop_back();
		SDL_FreeSurface(page->bdr.s);
		page->bdr = editgui_listmaps(page->things, page->font, page->gd->map->name);
		break;
	case EGLOAD: // load map or new map
		if (loadmap(page->gd->map, page->things[EGNAME].value)) return 0;
	case EGNEWMAP:
		page->gd->map->name = (idx == EGNEWMAP) ? "newmap" : page->things[EGNAME].value;
		if (idx == EGNEWMAP) newmap(page->gd->map);
		break;
	default: // select map
		page->things[EGNAME].value = thing->value;
		redrawinput(page->font, &page->things[EGNAME], false);
		break;
	}
	return 0;
}
void editgui_update(GUIPage* page, int dt)
{
	// redraw backdrop if the mapname changed
	if (page->bdr.value == page->gd->map->name) return;
	SDL_FreeSurface(page->bdr.s);
	page->bdr = backdrop(
		page->things,
		page->font, page->gd->map->name.c_str(),
		5, 5, page->bdr.border, page->bdr.bg, page->bdr.fg
	);
}
void setup_editgui(SETUP_ARGS)
{
	page.font = font; page.gd = gd;
	page.button_click = editgui_click;
	page.update = editgui_update;
	// reserve enough memory so that the pointers remain valid
	page.things.reserve(5);
	// add things and ste up pointers
	GUIThing* ref = addthing(page.things, inputbox(font, "Map name:", 100));
	addthing(page.things, button(font, "Save"), ALIGN_RIGHT);
	addthing(page.things, button(font, "Load"), ALIGN_LEFT, &ref->r);
	ref = addthing(page.things, button(font, "Wall Editor"), ALIGN_CENTER, &ref->r);
	addthing(page.things, button(font, "New Map"), ALIGN_CENTER, &ref->r);
	page.bdr = editgui_listmaps(page.things, font, gd->map->name);
	center_page(page, { draw_w / 2, draw_h / 2 });
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
int wallgui_click(GUIPage* page, GUIThing* thing)
{
	int i = thing - &page->things[WGS];
	// refresh buttons
	if (thing == &page->things.back())
	{
		for (int j = 0; j < page->gd->map->w * page->gd->map->h; j++)
			wallbutton_update(&page->things[WGS + j], page->gd->map->data[j].clip);
		return 0;
	}
	page->gd->map->data[i].clip = thing->overflown = !thing->overflown;
	if (page->gd->map->data[i].n == 0) set_faces(&page->gd->map->data[i], 1, 1, 1, 1);
	wallbutton_update(thing);
	return 0;
}
void wallgui_update(GUIPage* page, int dt)
{
	GETDATA(wallgui_data);
	// highlight tile that is being looked at
	int new_hl = page->gd->hl.x + page->gd->hl.y * page->gd->map->w;
	wallbutton_update(&page->things[WGS + data->last_hl]);
	if (page->gd->editmode && withinmap(page->gd->map, page->gd->hl.x, page->gd->hl.y))
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
	Uint32 color = SDL_MapRGBA(surface->format, 0xd0, 0, 0xff, 0xd0);
	SDL_Rect r = { (int) (page->gd->pos.x * 10), (int) (page->gd->pos.y * 10), 7, 7 };
	r.x += page->things[WGS].r.x;
	r.y += page->things[WGS].r.y;
	int len = 10, stroke = 3;
	// if facing > 112.5 deg: -1; < 22.5 deg: 1; 0; but don't care about top or bottom hemi
	// above, but using unit circle magic
	int ix = page->gd->fieldcenter.x / SQRT2OVER4;
	if (ix > 1 || ix < -1) ix = (ix < 0) ? -1 : 1;
	int iy = page->gd->fieldcenter.y / SQRT2OVER4;
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
void setup_wallgui(SETUP_ARGS)
{
	page.font = font; page.gd = gd;
	wallgui_data* ptrs = new wallgui_data;
	page.userdata = ptrs;
	page.button_click = wallgui_click;
	page.update = wallgui_update;
	page.draw = wallgui_draw;
	page.things.reserve(gd->map->w * gd->map->h + 1);
	for (int y = 0; y < gd->map->h; y++)
	{
		for (int x = 0; x < gd->map->w; x++)
		{
			GUIThing* g;
			if (!x && y) g = addthing(
				page.things, button(10, 10, GRAY(0xff)),
				ALIGN_LEFT, &page.things[(y - 1) * gd->map->w].r, 0
			);
			else g = addthing(
				page.things, button(10, 10, GRAY(0xff)),
				ALIGN_TOP, NULL, 0
			);
			wallbutton_update(g, wall_at(gd->map, x, y)->clip);
		}
	}
	addthing(page.things, button(font, "Refresh"), ALIGN_RIGHT);
	page.bdr = backdrop(page.things, font, "Wall Editor", 10, 10);
	center_page(page, { draw_w / 2, draw_h / 2 });
}
