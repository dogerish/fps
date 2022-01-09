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

// show available maps, returns new backdrop
GUIThing mapcolumns(std::vector<GUIThing> &guithings, TTF_Font* font, maingui_data &ptrs)
{
	std::vector<std::string> entries;
	for (auto entry : fs::directory_iterator("maps/"))
		entries.push_back(entry.path().filename().string());
	columnate(
		guithings, font, "Maps", entries,
		{ ptrs.namebox->r.x, ptrs.loadbut->r.y, ptrs.namebox->r.w, ptrs.loadbut->r.h },
		5, 5,
		GRAY(0xff), DGRAY(0xff), GRAY(0xff)
	);
	// update pointers
	ptrs.loadbut = (ptrs.savebut = (ptrs.namebox = &guithings[0]) + 1) + 1;
	return backdrop(guithings, font, ptrs.mapname->c_str());
}

void mgui_click(TTF_Font* font, GUIPage* page, GUIThing* thing)
{
	maingui_data* data = (maingui_data*) page->userdata;
	// save map
	if (thing == data->savebut && !savemap(data->map, data->namebox->value))
	{
		// delete everything after maps title
		for (int i = page->things.size() - 1; page->things[i].type != GUI_TEXT; i--)
		{
			SDL_FreeSurface(page->things[i].s);
			page->things.pop_back();
		}
		SDL_FreeSurface(page->things.back().s);
		page->things.pop_back();
		SDL_FreeSurface(page->bdr.s);
		page->bdr = mapcolumns(page->things, font, *data);
	}
	// load map
	else if (thing == data->loadbut && !loadmap(data->map, data->namebox->value))
	{
		SDL_FreeSurface(page->bdr.s);
		page->bdr = backdrop(
			page->things,
			font, (*data->mapname = data->namebox->value).c_str(),
			5, 5, page->bdr.border, page->bdr.bg, page->bdr.fg
		);
	}
	// select map
	else if (thing != data->loadbut && thing != data->savebut)
	{
		data->namebox->value = thing->value;
		redrawinput(font, data->namebox, false);
	}
}
