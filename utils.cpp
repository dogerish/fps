#include "utils.h"
#ifdef __linux__
#include <SDL2/SDL.h>
#endif
#ifdef __APPLE__
#include <SDL.h>
#endif

int init(SDL_Window* &window, SDL_Surface* &surface)
{
	return (
		   SDL_Init(SDL_INIT_VIDEO)
		|| (window = SDL_CreateWindow("raycaster", 0, 0, WIDTH, HEIGHT, 0)) == NULL
		|| (surface = SDL_GetWindowSurface(window)) == NULL
	) * -1;
}

void quit(
	SDL_Surface* textures[], SDL_Surface* floortex, SDL_Surface* ceiltex, SDL_Surface* ch,
	SDL_Window* window, SDL_Surface* surface
)
{
	for (int i = 0; i < 15; i++) SDL_FreeSurface(textures[i]);
	SDL_FreeSurface(floortex);
	SDL_FreeSurface(ceiltex);
	SDL_FreeSurface(surface);
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
	err |= (floortex = SDL_LoadBMP("textures/floor.bmp")) == NULL;
	SDL_LockSurface(floortex);
	err |= (ceiltex = SDL_LoadBMP("textures/ceiling.bmp")) == NULL;
	SDL_LockSurface(ceiltex);
	err |= (ch = SDL_LoadBMP("textures/crosshair.bmp")) == NULL;
	return err * -1;
}
