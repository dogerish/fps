#ifndef RCMAP
#define RCMAP
#include <string>

#define MAPVER 2

struct Wall {
	unsigned int n:4;
	unsigned int w:4;
	unsigned int s:4;
	unsigned int e:4;
	bool clip:1;
};

const Wall NULL_WALL = { 1, 1, 1, 1, 1 };

struct Map {
	int w, h;
	int loaded = 0;
	std::string name;
	Wall* data;
};

Map* create_map(int w, int h);
void free_map(Map* map);
bool withinmap(Map* map, int x, int y);
Wall* wall_at(Map* map, int x, int y);

// gets and/or sets the face value; returns -1 on invalid face
int face_at(Wall* wall, int face);
int face_at(Wall* wall, int face, int value);
// sets all the faces at once
void set_faces(Wall* wall, int s, int e, int n, int w);

void newmap(Map* map);
// returns nonzero on failure; error in SDL_GetError()
int loadmap(Map* map, std::string name, bool reset_on_fail = false);
int savemap(Map* map, std::string name);

#endif
