#include "utils.h"
#include SDL2_H
#include SDL2_IMG_H
#include <cstdio>

int init(SDL_Window* &window, SDL_Surface* &surface, const char* title, int w, int h)
{
	int err = 0;
	err |= SDL_Init(SDL_INIT_VIDEO);
	err |= (window = SDL_CreateWindow(
			title,
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h,
			SDL_WINDOW_RESIZABLE
		)) == NULL;
	err |= (surface = SDL_GetWindowSurface(window)) == NULL;
	if (surface) SDL_SetSurfaceRLE(surface, 1);
	return err;
}

void quit(
	SDL_Surface* titletex,
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
	SDL_Surface* tmp = IMG_Load(file);
	if (!tmp) return NULL;
	SDL_Surface* s = SDL_ConvertSurfaceFormat(tmp, fmt, 0);
	SDL_FreeSurface(tmp);
	return s;
}

// load textures; 0 on success
int loadtex(
	Uint32 fmt,
	SDL_Surface* &titletex,
	SDL_Surface* textures[],
	SDL_Surface* &floortex,
	SDL_Surface* &ceiltex,
	SDL_Surface* &ch
)
{
	int err = 0;
	err |= (titletex = loadwithfmt("textures/title.png", fmt)) == NULL;
	char filename[20];
	for (int i = 0; i < 15; i++)
	{
		sprintf(filename, "textures/%i.png", i + 1);
		err |= (textures[i] = loadwithfmt(filename, fmt)) == NULL;
		if (textures[i]) SDL_SetSurfaceRLE(textures[i], 1);
	}
	err |= (floortex = loadwithfmt("textures/floor.png", fmt))   == NULL;
	err |= (ceiltex  = loadwithfmt("textures/ceiling.png", fmt)) == NULL;
	if (floortex) { SDL_SetSurfaceRLE(floortex, 0); SDL_LockSurface(floortex); }
	if (ceiltex)  { SDL_SetSurfaceRLE(ceiltex,  0); SDL_LockSurface(ceiltex);  }
	err |= (ch = IMG_Load("textures/crosshair.png")) == NULL;
	if (ch) SDL_SetSurfaceRLE(ch, 1);
	return err;
}
