#include "map.h"
#include <stdlib.h>
#include "utils.h"
#include SDL2_H
#include <string>
#include <fstream>

Map* create_map(int w, int h)
{
	Map* map = new Map;
	map->w = w;
	map->h = h;
	map->data = (Wall*) malloc(sizeof(Wall) * w * h);
	return map;
}

void free_map(Map* map)
{
	free(map->data);
	free(map);
	map = NULL;
}

bool withinmap(Map* map, int x, int y) { return y >= 0 && y < map->h && x >= 0 && x < map->w; }
Wall nullcopy;
Wall* wall_at(Map* map, int x, int y)
{ return withinmap(map, x, y) ? &map->data[y * map->w + x] : &(nullcopy = NULL_WALL); }

int face_at(Wall* wall, int face)
{
	switch (face % 4)
	{
	case 0: return wall->s;
	case 1: return wall->e;
	case 2: return wall->n;
	case 3: return wall->w;
	default: return -1;
	}
}
int face_at(Wall* wall, int face, int value)
{
	switch (face)
	{
	case 0: return wall->s = value;
	case 1: return wall->e = value;
	case 2: return wall->n = value;
	case 3: return wall->w = value;
	default: return -1;
	}
}

void set_faces(Wall* wall, int s, int e, int n, int w)
{
	wall->s = s;
	wall->e = e;
	wall->n = n;
	wall->w = w;
}

int loadmap(Map* map, std::string name, bool reset_on_fail)
{
	int e = 0, v;
	name = "maps/" + name;
	std::ifstream file(name, std::ios::binary);
	file >> v;
	if (v < 1 || v > MAPVER && ++e)
		SDL_SetError("Invalid map version. Expected %i or less, but got %i", MAPVER, v);
	else
	{
		file.ignore();
		for (int i = 0; i < map->h * map->w; i++)
		{
			char se = file.get(), nw = file.get();
			set_faces(&map->data[i], se & 0xf, se >> 4, nw & 0xf, nw >> 4);
			if (v == 1) map->data[i].clip = nw || se;
			else map->data[i].clip = file.get();
		}
	}
	if (!file.good() && ++e)
	{
		SDL_SetError("Error while loading '%s'", name.c_str());
		// set to default map
		if (reset_on_fail)
			for (int y = 0; y < map->h; y++) for (int x = 0; x < map->w; x++)
			{
				set_faces(wall_at(map, x, y), 1, 1, 1, 1);
				wall_at(map, x, y)->clip = !(x % (map->w - 1) && y % (map->h - 1));
			}
	}
	file.close();
	return e;
}

int savemap(Map* map, std::string name)
{
	int e = 0;
	name = "maps/" + name;
	std::ofstream file(name, std::ios::binary);
	file << MAPVER << std::endl;
	for (int i = 0; i < map->w * map->h; i++)
	{
		file.put(map->data[i].s | map->data[i].e << 4);
		file.put(map->data[i].n | map->data[i].w << 4);
		file.put(map->data[i].clip);
	}
	if (!file.good() && ++e) SDL_SetError("Error while saving '%s'", name.c_str());
	file.close();
	return e;
}
