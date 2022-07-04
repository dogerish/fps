#ifndef RC_GAME
#define RC_GAME
#include <SDL.h>
#include <string>
#include "vec.h"
#include "map.h"

enum GameMode { GM_TITLESCREEN, GM_EDITING, GM_PLAYING };

// variables for game
struct GameData {
	std::string resource_path = "resources";
	Vec2d<float> pos;
	float fov;
	// normalized coordinates from leftmost fov to rightmost
	Vec2d<float> fieldleft, fieldcenter, fieldright;
	int gamemode;
	bool editmode; Vec2d<int> hl;
	Map* map;
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
