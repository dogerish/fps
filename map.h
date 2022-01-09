#ifndef RCMAP
#define RCMAP
#include <string>

#define MAPVER 1

struct Wall {
	unsigned int n:4;
	unsigned int w:4;
	unsigned int s:4;
	unsigned int e:4;
	unsigned int clip:1;
};

struct Map {
	int w, h;
	Wall* data;
};

Map* create_map(int w, int h);
void free_map(Map* map);
Wall* wall_at(Map* map, int x, int y);

// gets and/or sets the face value; returns -1 on invalid face
int face_at(Wall* wall, int face);
int face_at(Wall* wall, int face, int value);
// sets all the faces at once
void set_faces(Wall* wall, int s, int e, int n, int w);

// returns nonzero on failure; error in SDL_GetError()
int loadmap(Map* map, std::string name, bool reset_on_fail = false);
int savemap(Map* map, std::string name);

#endif
