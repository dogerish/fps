#ifndef RC_GAME
#define RC_GAME
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <vector>
#include "vec.h"
#include "map.h"

enum GameMode { GM_TITLESCREEN, GM_EDITING, GM_PLAYING };

// variables for game
struct GameData {
	SDL_Window* window = NULL;
	SDL_Surface* surface = NULL;
	TTF_Font* font = NULL;
	std::string resource_path = "resources";
	Vec2d<float> pos;
	float fov;
	// normalized coordinates from leftmost fov to rightmost
	Vec2d<float> fieldleft, fieldcenter, fieldright;
	int gamemode;
	int textediting = 0;
	bool editmode; Vec2d<int> hl;
	Map* map;
	// gui page id for current and history
	int page;
	std::vector<int> guihistory;
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
