#include "maingui.h"
#include <SDL_ttf.h>
#include <string>
#include <vector>
#include <filesystem>
#include "gui.h"
#include "guipage.h"
#include "map.h"
#include "game.h"
namespace fs = std::filesystem;

#define GETDATA(type) type* data = (type*) userdata

void mapcolumns(
	GameData& gd,
	std::vector<GUIThing> &guithings,
	int numcols, SDL_Rect ref
)
{
	std::vector<std::string> entries;
	for (auto entry : fs::directory_iterator(gd.resource_path + "/maps/"))
		entries.push_back(entry.path().filename().string());
	columnate(
		gd, guithings, "Maps", entries, numcols, ref,
		5, 5, GRAY(0xff), DGRAY(0xff), GRAY(0xff)
	);
}

TitleGUI::TitleGUI(SETUP_ARGS)
{
	this->id = id;
	this->name = name;
	things.reserve(5);
	GUIThing* tmp = addthing(gd, things, button(gd, "Home"));
	tmp->onclick = "gamemode titlescreen";
	SDL_Rect leftref = tmp->r;
	tmp = addthing(gd, things, button(gd, "Play"), ALIGN_TOP);
	tmp->onclick = "gamemode playing";
	SDL_Rect ref = tmp->r;
	addthing(gd, things, button(gd, "Edit"), ALIGN_TOP)->onclick = "gamemode editing";
	addthing(gd, things, button(gd, "Select Map"), ALIGN_CENTER, &ref)->onclick = "closegui; showgui mapsel";
	addthing(gd, things, button(gd, "Edit..."), ALIGN_CENTER, &ref)->onclick = "showgui edit";
	tmp = addthing(gd, things, button(gd, "Settings"));
	tmp->onclick = "showgui settings";
	ref = tmp->r;
	ref = { leftref.x, ref.y, leftref.w, ref.h };
	addthing(gd, things, button(gd, "Exit", 5, 1, RED(0xff)), ALIGN_LEFT, &ref, 15)->onclick = "quit";
	bdr = backdrop(gd, things, "Main Menu");
	center_page(gd);
}

void TitleGUI::update(GameData& gd, int dt)
{
	things[TGHOME].shown = gd.gamemode != GM_TITLESCREEN;
	things[TGPLAY].shown = gd.gamemode != GM_PLAYING;
	things[TGEDIT].shown = gd.gamemode != GM_EDITING;
	things[TGSELMAP].shown  = gd.gamemode != GM_EDITING && gd.gamemode != GM_TITLESCREEN;
	things[TGEDITMAP].shown = gd.gamemode == GM_EDITING;
}

MapSelGUI::MapSelGUI(SETUP_ARGS)
{
	this->id = id;
	this->name = name;
	GUIThing refresh_button = button(gd, "Refresh");
	refresh_button.id = 0;
	refresh_button.onclick = "refreshgui mapsel";
	addthing(gd, things, refresh_button);
	bdr.value = "Map Selector";
	refresh(gd);
	center_page(gd);
}

void MapSelGUI::refresh(GameData& gd)
{
	for (GUIThing &g : things) if (g.id != 0) SDL_FreeSurface(g.s);
	if (things.size() > 1) things.erase(things.begin() + 1, things.end());
	mapcolumns(gd, things, 5, things.back().r);
	// add onclick commands to maps
	for (GUIThing &g : things)
		if (g.id < 0)
			g.onclick = "loadmap " + gd.cmdhand->escapestr(g.value) + "; closegui";
	SDL_Rect ref = things.back().r;
	if (bdr.s) SDL_FreeSurface(bdr.s);
	bdr = backdrop(gd, things, bdr.value.c_str());
	center_page(gd);
}

int MapSelGUI::page_close(GameData& gd)
{
	if (!gd.map->loaded) gd.gamemode = GM_TITLESCREEN;
	return 0;
}

EditGUI::EditGUI(SETUP_ARGS)
{
	this->id = id;
	this->name = name;
	// reserve enough memory so that the pointers remain valid
	things.reserve(5);
	// add things and ste up pointers
	GUIThing* ref = addthing(gd, things, inputbox(gd, "Map name:", 100));
	addthing(gd, things, button(gd, "Save"), ALIGN_RIGHT)->onclick = "savemap (editguimapname)";
	addthing(gd, things, button(gd, "Load"), ALIGN_LEFT, &ref->r)->onclick = "loadmap (editguimapname)";
	ref = addthing(gd, things, button(gd, "Wall Editor"), ALIGN_CENTER, &ref->r);
	ref->onclick = "showgui wall";
	addthing(gd, things, button(gd, "New Map"), ALIGN_CENTER, &ref->r)->onclick = "newmap";
	bdr = listmaps(gd);
	center_page(gd);
}

// show available maps, returns new backdrop
GUIThing EditGUI::listmaps(GameData& gd)
{
	SDL_Rect ref = things.back().r;
	ref.x = things[EGNAME].r.x;
	ref.w = things[EGNAME].r.w;
	mapcolumns(gd, things, 3, ref);
	// add onclick commands to map buttons
	for (int i = 5; i < things.size(); i++)
		things[i].onclick = std::string("editguimapname ")
			+ gd.cmdhand->escapestr(things[i].value);
	return backdrop(gd, things, gd.map->name.c_str());
}

void EditGUI::refresh(GameData& gd)
{
	// update map listing
	for (int i = things.size() - 1; things[i].type != GUI_TEXT; i--)
	{
		SDL_FreeSurface(things[i].s);
		things.pop_back();
	}
	SDL_FreeSurface(things.back().s);
	things.pop_back();
	SDL_FreeSurface(bdr.s);
	bdr = listmaps(gd);
}

void EditGUI::update(GameData& gd, int dt)
{
	// redraw backdrop if the mapname changed
	if (bdr.value == gd.map->name) return;
	SDL_FreeSurface(bdr.s);
	bdr = backdrop(
		gd,
		things,
		gd.map->name.c_str(),
		5, 5, bdr.border, bdr.bg, bdr.fg
	);
}

WallGUI::WallGUI(SETUP_ARGS)
{
	this->id = id;
	this->name = name;
	wallgui_data* ptrs = new wallgui_data;
	userdata = ptrs;
	things.reserve(gd.map->w * gd.map->h + 1);
	for (int y = 0; y < gd.map->h; y++)
	{
		for (int x = 0; x < gd.map->w; x++)
		{
			GUIThing* g;
			if (!x && y) g = addthing(
				gd,
				things, button(gd, 10, 10, GRAY(0xff)),
				ALIGN_LEFT, &things[(y - 1) * gd.map->w].r, 0
			);
			else g = addthing(
				gd,
				things, button(gd, 10, 10, GRAY(0xff)),
				ALIGN_TOP, NULL, 0
			);
			// toggle related wall when clicked with command: wall $x $y -1
			g->onclick = std::string("wall ")
				+ std::to_string(x) + " "
				+ std::to_string(y) + " -1";
			WallGUI::button_update(gd, g, wall_at(gd.map, x, y)->clip);
		}
	}
	addthing(gd, things, button(gd, "Refresh"), ALIGN_RIGHT)->onclick = "refreshgui wall";
	bdr = backdrop(gd, things, "Wall Editor", 10, 10);
	center_page(gd);
}

void WallGUI::button_update(GameData& gd, GUIThing* g)
{
	Uint8 v = 0xff - g->overflown * 0x80;
	SDL_SetSurfaceColorMod(g->s, v, v, v);
}

void WallGUI::button_update(GameData& gd, GUIThing* g, int overflown)
{
	g->overflown = overflown;
	WallGUI::button_update(gd, g);
}

void WallGUI::refresh(GameData& gd)
{
	for (int j = 0; j < gd.map->w * gd.map->h; j++)
		WallGUI::button_update(gd, &things[WG_WALLSTART + j], gd.map->data[j].clip);
}

void WallGUI::update(GameData& gd, int dt)
{
	GETDATA(wallgui_data);
	// highlight tile that is being looked at
	int new_hl = gd.hl.x + gd.hl.y * gd.map->w;
	WallGUI::button_update(gd, &things[WG_WALLSTART + data->last_hl]);
	if (gd.editmode && withinmap(gd.map, gd.hl.x, gd.hl.y))
	{
		data->last_hl = new_hl;
		SDL_SetSurfaceColorMod(things[WG_WALLSTART + data->last_hl].s, 0xff, 0, 0xff);
	}
}

#define SQRT2OVER4 0.3535533906 // sqrt(2) / 4
void WallGUI::draw(GameData& gd)
{
	GUIPage::draw(gd);
	// draw player
	Uint32 color = SDL_MapRGBA(gd.surface->format, 0xd0, 0, 0xff, 0xd0);
	SDL_Rect r = { (int) (gd.pos.x * 10), (int) (gd.pos.y * 10), 7, 7 };
	r.x += things[WG_WALLSTART].r.x;
	r.y += things[WG_WALLSTART].r.y;
	int len = 10, stroke = 3;
	// if facing > 112.5 deg: -1; < 22.5 deg: 1; 0; but don't care about top or bottom hemi
	// above, but using unit circle magic
	int ix = gd.fieldcenter.x / SQRT2OVER4;
	if (ix > 1 || ix < -1) ix = (ix < 0) ? -1 : 1;
	int iy = gd.fieldcenter.y / SQRT2OVER4;
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
		SDL_FillRect(gd.surface, &l, color);
	}
	// diagonal line
	else
	{
		SDL_Rect rects[(len *= 0.7071067812)];
		for (int i = 0; i < len; i++)
			rects[i] = { r.x + i * ix - stroke / 2, r.y + i * iy - stroke / 2, stroke, stroke };
		SDL_FillRects(gd.surface, rects, len, color);
	}
	// finish drawing player dot
	r.x -= r.w / 2; r.y -= r.w / 2;
	SDL_FillRect(gd.surface, &r, color);
}

CommandGUI::CommandGUI(SETUP_ARGS)
{
	this->id = id;
	this->name = name;
	addthing(gd, things, inputbox(gd, "Command:", 400));
	addthing(gd, things, button(gd, "Run"), ALIGN_RIGHT)->onclick = "eval (commandguicmd)";
	bdr = backdrop(gd, things, "Command Interface");
	center_page(gd);
}
