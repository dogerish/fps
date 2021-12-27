#include "utils.h"
#ifdef __linux__
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#endif
#ifdef __APPLE__
#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#endif

int init(SDL_Window** window, SDL_Renderer** renderer)
{
	return (
		   SDL_Init(SDL_INIT_VIDEO)
		|| SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, window, renderer)
	) * -1;
}

void quit(
	SDL_Texture* textures[], SDL_Texture* floortex, SDL_Texture* ceiltex,
	SDL_Window* window, SDL_Renderer* renderer
)
{
	for (int i = 0; i < 15; i++) SDL_DestroyTexture(textures[i]);
	SDL_DestroyTexture(floortex);
	SDL_DestroyTexture(ceiltex);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

// load textures; 0 on success
int loadtex(
	SDL_Renderer* renderer,
	SDL_Texture* textures[],
	SDL_Texture* &floortex,
	SDL_Texture* &ceiltex
)
{
	int err = 0;
	char filename[20];
	SDL_Surface* surface;
	for (int i = 0; i < 15; i++)
	{
		sprintf(filename, "textures/%i.bmp", i + 1);
		err |= (surface = SDL_LoadBMP(filename)) == NULL;
		err |= (textures[i] = SDL_CreateTextureFromSurface(renderer, surface)) == NULL;
		SDL_FreeSurface(surface);
	}
	err |= (surface = SDL_LoadBMP("textures/floor.bmp")) == NULL;
	err |= (floortex = SDL_CreateTextureFromSurface(renderer, surface)) == NULL;
	SDL_FreeSurface(surface);
	err |= (surface = SDL_LoadBMP("textures/ceiling.bmp")) == NULL;
	err |= (ceiltex = SDL_CreateTextureFromSurface(renderer, surface)) == NULL;
	SDL_FreeSurface(surface);
	return err * -1;
}
