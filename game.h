#ifndef RC_GAME
#define RC_GAME
#include "rays.h"
#include "map.h"

enum GameMode { GM_TITLESCREEN, GM_EDITING, GM_PLAYING };

// variables for game
struct GameData {
	Vec2d<float> pos;
	float fov;
	// normalized coordinates from leftmost fov to rightmost
	Vec2d<float> fieldleft, fieldcenter, fieldright;
	int gamemode;
	bool editmode; Vec2d<int> hl;
	Map* map;
};

#endif
