#include "utils.h"
#include SDL2_H
#include <cstdio>

int init(SDL_Window* &window, SDL_Surface* &surface)
{
	int err = 0;
	err |= SDL_Init(SDL_INIT_VIDEO);
	err |= (window = SDL_CreateWindow(
			"raycaster",
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT,
			0
		)) == NULL;
	err |= (surface = SDL_GetWindowSurface(window)) == NULL;
	if (surface) SDL_SetSurfaceRLE(surface, 1);
	return err;
}

void quit(
	SDL_Surface* textures[], SDL_Surface* floortex, SDL_Surface* ceiltex, SDL_Surface* ch,
	SDL_Window* window
)
{
	for (int i = 0; i < 15; i++) SDL_FreeSurface(textures[i]);
	SDL_FreeSurface(floortex);
	SDL_FreeSurface(ceiltex);
	SDL_FreeSurface(ch);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

SDL_Surface* loadwithfmt(const char* file, Uint32 fmt)
{
	SDL_Surface* tmp = SDL_LoadBMP(file);
	if (!tmp) return NULL;
	SDL_Surface* s = SDL_ConvertSurfaceFormat(tmp, fmt, 0);
	SDL_FreeSurface(tmp);
	return s;
}

// load textures; 0 on success
int loadtex(
	Uint32 fmt,
	SDL_Surface* textures[],
	SDL_Surface* &floortex,
	SDL_Surface* &ceiltex,
	SDL_Surface* &ch
)
{
	int err = 0;
	char filename[20];
	for (int i = 0; i < 15; i++)
	{
		sprintf(filename, "textures/%i.bmp", i + 1);
		err |= (textures[i] = loadwithfmt(filename, fmt)) == NULL;
		if (textures[i]) SDL_SetSurfaceRLE(textures[i], 1);
	}
	err |= (floortex = loadwithfmt("textures/floor.bmp", fmt))   == NULL;
	err |= (ceiltex  = loadwithfmt("textures/ceiling.bmp", fmt)) == NULL;
	if (floortex) { SDL_SetSurfaceRLE(floortex, 0); SDL_LockSurface(floortex); }
	if (ceiltex)  { SDL_SetSurfaceRLE(ceiltex,  0); SDL_LockSurface(ceiltex);  }
	err |= (ch = loadwithfmt("textures/crosshair.bmp", fmt)) == NULL;
	return err;
}
