#ifndef RC_GAME
#define RC_GAME
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <vector>
#include <fstream>
#include "vec.h"
#include "map.h"
#include "command.h"

enum GameMode { GM_TITLESCREEN, GM_EDITING, GM_PLAYING };

// variables for game
class GameData {
	public:
		std::ofstream logfile;
		SDL_Window* window = NULL;
		SDL_Surface* surface = NULL;
		CommandHandler* cmdhand = NULL;
		TTF_Font* font = NULL;
		Map* map;

		std::string resource_path = "resources";

		Vec2d<float> pos;
		float fov;
		float fps = 0;
		// normalized coordinates from leftmost fov to rightmost
		Vec2d<float> fieldleft, fieldcenter, fieldright;

		int gamemode;
		bool textediting = false, editmode = false; Vec2d<int> hl;
		// gui page id for current and history
		int page;
		std::vector<int> guihistory;


		GameData(const char* logfilename = "logfile.txt");
		~GameData();
		void log(std::string message);
};

struct GameTextures
{
	SDL_Surface* title;     // title screen
	SDL_Surface* wall[15];  // wall textures
	SDL_Surface* floor;     // floor texture
	SDL_Surface* ceiling;   // ceiling texture
	SDL_Surface* crosshair; // crosshair
};

#endif
