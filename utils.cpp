#include "utils.h"
#include SDL2_H

int init(SDL_Window* &window, SDL_Surface* &surface)
{
	return (
		   SDL_Init(SDL_INIT_VIDEO)
		|| (window = SDL_CreateWindow(
			"raycaster",
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT,
			0
		)) == NULL
		|| (surface = SDL_GetWindowSurface(window)) == NULL
	) * -1;
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

// load textures; 0 on success
int loadtex(
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
		err |= (textures[i] = SDL_LoadBMP(filename)) == NULL;
	}
	SDL_Surface* loadedsurf = SDL_LoadBMP("textures/floor.bmp");
	err |= loadedsurf == NULL;
	if (loadedsurf != NULL)
	{
		floortex = SDL_CreateRGBSurface(0, loadedsurf->w, loadedsurf->h, 32, 0, 0, 0, 0);
		SDL_BlitSurface(loadedsurf, NULL, floortex, NULL);
		SDL_FreeSurface(loadedsurf);
	}
	err |= (loadedsurf = SDL_LoadBMP("textures/ceiling.bmp")) == NULL;
	if (loadedsurf != NULL)
	{
		ceiltex = SDL_CreateRGBSurface(0, loadedsurf->w, loadedsurf->h, 32, 0, 0, 0, 0);
		SDL_BlitSurface(loadedsurf, NULL, ceiltex, NULL);
		SDL_FreeSurface(loadedsurf);
	}
	err |= (ch = SDL_LoadBMP("textures/crosshair.bmp")) == NULL;
	return err * -1;
}
