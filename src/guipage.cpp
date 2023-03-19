#include "guipage.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include "gui.h"
#include <vector>
#include <string>

bool isnumerical(char c) { return c >= '0' && c <= '9' || c == '.' || c == '-'; }

void GUIPage::button_click(GameData& gd, GUIThing* thing)
{
	std::string result;
	if (gd.cmdhand->run(thing->onclick, result) == -1)
		gd.log(gd.cmdhand->geterror());
}

int GUIPage::page_close(GameData& gd) { return 0; }

void GUIPage::update(GameData& gd, int dt) { }

void GUIPage::draw(GameData& gd)
{
	SDL_Rect copy = bdr.r;
	SDL_BlitSurface(bdr.s, NULL, gd.surface, &copy);
	for (GUIThing g : things)
		if (g.shown) SDL_BlitSurface(g.s, NULL, gd.surface, &(copy = g.r));
}

void GUIPage::refresh(GameData& gd) {}

void GUIPage::center_page(GameData& gd)
{
	SDL_Point center = { gd.surface->w / 2, gd.surface->h / 2 };
	// calculate the offset from the current center and move everything by that offset
	center.x -= bdr.r.x + bdr.r.w / 2;
	center.y -= bdr.r.y + bdr.r.h / 2;
	bdr.r.x += center.x;
	bdr.r.y += center.y;
	for (GUIThing &g : things) { g.r.x += center.x; g.r.y += center.y; }
}

void GUIPage::startinput(GameData& gd, GUIThing* box)
{
	SDL_StartTextInput();
	focused = box;
	gd.textediting = 1;
	redrawinput(gd, box);
}
void GUIPage::stopinput(GameData& gd)
{
	SDL_StopTextInput();
	gd.textediting = 0;
	focused->overflown = 0;
	redrawinput(gd, focused);
	focused = NULL;
}

void GUIPage::onclick(GameData& gd, SDL_Point mouse)
{
	if (focused) stopinput(gd);
	// close if click happened outside of backdrop
	if (!SDL_PointInRect(&mouse, &bdr.r))
	{
		gd.cmdhand->call(std::vector<std::string> { "closegui" });
		return;
	}
	for (GUIThing &g : things)
	{
		if (!g.shown || !SDL_PointInRect(&mouse, &g.r)) continue;
		if      (g.type == GUI_INPUT)  startinput(gd, &g);
		else if (g.type == GUI_BUTTON)
		{
			button_click(gd, &g);
			return;
		}
		else continue;
		break;
	}
}

int GUIPage::onkeypress(GameData& gd, SDL_Keycode key)
{
	if (!focused) return 1 - (key == SDLK_ESCAPE) * 2;
	switch (key)
	{
	case SDLK_ESCAPE:
	case SDLK_RETURN: stopinput(gd); break;
	case SDLK_BACKSPACE:
		if (!focused->overflown && focused->value.size())
			focused->value.pop_back();
		focused->overflown = 0;
		redrawinput(gd, focused);
		break;
	}
	return 0;
}

int GUIPage::oninput(GameData& gd, const char* text)
{
	if (!focused || focused->overflown & 1) return 1;
	focused->overflown &= ~2;
	int r = 0;
	// if this is a number input box, only accept numerical characters
	if (focused->subtype == GUIST_NUMINPUT && !isnumerical(*text))
	{
		focused->overflown |= 2;
		r = -1;
	}
	else focused->value += text;
	redrawinput(gd, focused);
	return r;
}

void GUIPage::drawgui(GameData& gd, int dt)
{
	update(gd, dt);
	draw(gd);
}

int GUIPage::closegui(GameData& gd)
{
	if (page_close(gd)) return -1;
	if (!gd.guihistory.size()) { gd.page = -1; return 0; }
	gd.page = gd.guihistory.back();
	gd.guihistory.pop_back();
	return 0;
}
void GUIPage::opengui(GameData& gd)
{
	if (gd.page >= 0) gd.guihistory.push_back(gd.page);
	gd.page = id;
}
