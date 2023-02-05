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

#define SETUP_ARGS GameData& gd, int id

// columns of available maps
void mapcolumns(
	GameData& gd,
	std::vector<GUIThing> &guithings,
	int numcols, SDL_Rect ref
);

/* ---------- title gui (0) ---------- */
class TitleGUI: public GUIPage
{
	public:
		TitleGUI(SETUP_ARGS);
		int button_click(GameData& gd, GUIThing* thing) override;
		void update(GameData& gd, int dt) override;
};

/* ---------- map selector gui (1) ---------- */
class MapSelGUI: public GUIPage
{
	public:
		MapSelGUI(SETUP_ARGS);
		void refresh(GameData& gd);
		int button_click(GameData& gd, GUIThing* thing) override;
		int page_close(GameData& gd) override;
};

/* ---------- edit gui (2) ---------- */
class EditGUI: public GUIPage
{
	public:
		EditGUI(SETUP_ARGS);
		GUIThing listmaps(GameData& gd);
		int button_click(GameData& gd, GUIThing* thing) override;
		void update(GameData& gd, int dt) override;
};

/* ---------- wall gui (3) ---------- */
struct wallgui_data {
	int last_hl = 0;
};
class WallGUI: public GUIPage
{
	public:
		WallGUI(SETUP_ARGS);
		static void button_update(GameData& gd, GUIThing* g);
		static void button_update(GameData& gd, GUIThing* g, int overflown);
		int button_click(GameData& gd, GUIThing* thing) override;
		void update(GameData& gd, int dt) override;
		void draw(GameData& gd) override;
};

#endif
