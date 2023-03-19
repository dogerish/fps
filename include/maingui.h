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

#define SETUP_ARGS GameData& gd, int id, std::string name

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
		enum TITLEGUI_INDICES { TGHOME, TGPLAY, TGEDIT, TGSELMAP, TGEDITMAP, TGSETTINGS, TGEXIT };

		TitleGUI(SETUP_ARGS);
		void update(GameData& gd, int dt) override;
};

/* ---------- map selector gui (1) ---------- */
class MapSelGUI: public GUIPage
{
	public:
		MapSelGUI(SETUP_ARGS);
		void refresh(GameData& gd) override;
		int page_close(GameData& gd) override;
};

/* ---------- edit gui (2) ---------- */
class EditGUI: public GUIPage
{
	public:
		enum EDITGUI_INDICES { EGNAME, EGSAVE, EGLOAD, EGWALL, EGNEWMAP };

		EditGUI(SETUP_ARGS);
		GUIThing listmaps(GameData& gd);
		void refresh(GameData& gd) override;
		void update(GameData& gd, int dt) override;
};

/* ---------- wall gui (3) ---------- */
struct wallgui_data {
	int last_hl = 0;
};
class WallGUI: public GUIPage
{
	public:
		enum WALLGUI_INDICES { WG_WALLSTART = 0 };

		WallGUI(SETUP_ARGS);
		static void button_update(GameData& gd, GUIThing* g);
		static void button_update(GameData& gd, GUIThing* g, int overflown);
		void refresh(GameData& gd) override;
		void update(GameData& gd, int dt) override;
		void draw(GameData& gd) override;
};

#endif
