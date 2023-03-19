#ifndef GUIPAGE
#define GUIPAGE
#include <SDL.h>
#include <SDL_ttf.h>
#include <vector>
#include <string>
#include "gui.h"
#include "game.h"

// true for characters 0-9, '.', and '-'
bool isnumerical(char c);

class GUIPage {
	public:
		int id;
		std::string name;
		GUIThing bdr;
		std::vector<GUIThing> things;
		GUIThing* focused = NULL;
		void* userdata = NULL;

		// see return code for onclick()
		virtual void button_click(GameData& gd, GUIThing* thing);
		// return non-zero if the page shouldn't close
		virtual int page_close(GameData& gd);
		// update the page
		virtual void update(GameData& gd, int dt);
		// draw page to surface
		virtual void draw(GameData& gd);
		// refresh the page
		virtual void refresh(GameData& gd);

		// centers the page and its contents around center of gd.surface
		void center_page(GameData& gd);

		void startinput(GameData& gd, GUIThing* box);
		void stopinput(GameData& gd);

		void onclick(GameData& gd, SDL_Point mouse);
		// returns 0 normally, 1 if the input should still be processed, and -1 if the page 
		// should close
		int onkeypress(GameData& gd, SDL_Keycode key);
		/* return code:
		   -1: input was ignored because the character isn't allowed
		    0: normal
		    1: input was ignored because of overflow or no focused input box
		*/
		int oninput(GameData& gd, const char* text);

		void drawgui(GameData& gd, int dt);

		// returns -1 if the page wasn't closed. normal return 0
		int  closegui(GameData& gd);
		void opengui(GameData& gd);

};

#endif
