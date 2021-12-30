#include <iostream>
#include <fstream>
#ifdef __linux__
#include <SDL2/SDL.h>
#include <SDL2/SDL2_rotozoom.h>
#include <SDL2/SDL_ttf.h>
#endif
#ifdef __APPLE__
#include <SDL.h>
#include <SDL2_rotozoom.h>
#include <SDL_ttf.h>
#endif
#include <cmath>
#include "utils.h"
#include "rays.h"

#define ERROR_RETURN(code) std::cout << SDL_GetError() << std::endl; return code;
#define FLCL_POINTERUP(surface) p = (Uint8*) (surface->pixels) + 3 * (src.y * TEXSIZE + src.x)
#define FLCL_DRAWPOINT(y) winpixels[y * WIDTH + dest.x] = SDL_MapRGBA(surface->format, *p*v, *(p+1)*v, *(p+2)*v, 0xff)

const float MAXDIST = hypot(MAPW, MAPH) * 0.9;
// 0 = empty; 4 bits to determine the texture of each face
// technically unfinished
#define MAPVER 1
Uint16 tiles[MAPW][MAPH];
SDL_Surface* textures[15];
SDL_Surface* floortex;
SDL_Surface* ceiltex;
SDL_Surface* ch; // crosshair
float fov = (M_PI / 2) / 2;

int filter(void* arg, SDL_Event* e)
{
	return e->type == SDL_QUIT ||
	       (e->type == SDL_KEYDOWN && !e->key.repeat);
}

int main(int argc, char* argv[])
{
	SDL_Window*  window;
	SDL_Surface* surface;
	if (init(window, surface) || TTF_Init()) { ERROR_RETURN(1); }
	SDL_SetEventFilter(filter, NULL);
	if (loadtex(textures, floortex, ceiltex, ch)) { ERROR_RETURN(1); }
	// load map
	{
		std::ifstream file("maps/maze", std::ios::binary);
		int v;
		file >> v;
		if (v != MAPVER)
			std::cerr
			<< "Invalid map version. Expected " << MAPVER
			<< ", but got " << v << std::endl;
		else
			file.ignore().read((char*) tiles, MAPH * MAPW * 2);
		file.close();
	}
	Vec2d<float> pos = { MAPW / 2.f, MAPH / 2.f, -1};
	float heading = 0;
	Vec2d<float> fieldleft, fieldcenter = { 1, 0, 1 }, fieldright;
	bool editmode = false;
	Vec2d<int> hl;
	TTF_Font* font = TTF_OpenFont("Sans.ttf", 12);
	if (!font) std::cout << SDL_GetError() << std::endl;
	SDL_Surface* text;
	char infostr[64];
	Uint32 lasttick = SDL_GetTicks();
	const Uint8* kb = SDL_GetKeyboardState(NULL);
	bool gotevent = false;
	for (SDL_Event e; e.type != SDL_QUIT; gotevent = SDL_PollEvent(&e))
	{
		float tdiff = (SDL_GetTicks() - lasttick);
		lasttick = SDL_GetTicks();
		// update player
		{
			float multiplier = (kb[SDL_SCANCODE_W] - kb[SDL_SCANCODE_S]) / 250.f;
			Vec2d<float> last = pos;
			pos.x += multiplier * tdiff * fieldcenter.x;
			if (tiles[(int) pos.y][(int) pos.x]) pos.x = last.x;
			pos.y += multiplier * tdiff * fieldcenter.y;
			if (tiles[(int) pos.y][(int) pos.x]) pos.y = last.y;
			heading += (kb[SDL_SCANCODE_D] - kb[SDL_SCANCODE_A]) * tdiff / 400.f;
			fieldleft   = { cos(heading - fov), sin(heading - fov), 1 };
			fieldcenter = { cos(heading),       sin(heading),       1 };
			fieldright  = { cos(heading + fov), sin(heading + fov), 1 };
			// highlight the face under the crosshair
			if (editmode) raycast(pos, fieldcenter, tiles, hl);
		}
		// render
		// ceiling and floor
		{
		SDL_LockSurface(surface);
		Uint32* winpixels = (Uint32*) surface->pixels;
		SDL_Point src, dest;
		Uint8* p;
		for (dest.y = HEIGHT / 2; dest.y < HEIGHT; dest.y++)
		{
			// distance to floor horizontally
			float dist = HEIGHT / (SQRT_2 * (dest.y - HEIGHT / 2));
			Vec2d<float> step = {
				dist / WIDTH * (fieldright.x - fieldleft.x),
				dist / WIDTH * (fieldright.y - fieldleft.y),
				-1
			};
			// get map coords
			Vec2d<float> mappos = {
				pos.x + dist * fieldleft.x,
				pos.y + dist * fieldleft.y,
				-1
			};
			float v = (0xff - 0xef * dist / MAXDIST) / 255.f;
			for (dest.x = 0; dest.x < WIDTH; dest.x++)
			{
				src.x = (int) ((mappos.x - (int) mappos.x) * TEXSIZE) & TEXSIZE - 1;
				src.y = (int) ((mappos.y - (int) mappos.y) * TEXSIZE) & TEXSIZE - 1;
				FLCL_POINTERUP(floortex); FLCL_DRAWPOINT(dest.y);
				FLCL_POINTERUP(ceiltex);  FLCL_DRAWPOINT((HEIGHT - dest.y));

				mappos.x += step.x; mappos.y += step.y;
			}
		}
		SDL_UnlockSurface(surface);
		}
		// walls
		{
			SDL_Surface *zoomed,
			*slice = SDL_CreateRGBSurfaceWithFormat(
				textures[0]->flags,
				1, TEXSIZE,
				textures[0]->format->BytesPerPixel,
				textures[0]->format->format
			);
			Vec2d<float> vel = fieldleft;
			Vec2d<float> step = {
				(fieldright.x - vel.x) / WIDTH,
				(fieldright.y - vel.y) / WIDTH,
				1
			};
			SDL_Rect src = { 0, 0, 1, TEXSIZE };
			for (int i = 0; i < WIDTH; i++)
			{
				src = { 0, 0, 1, TEXSIZE };
				vel.x += step.x; vel.y += step.y; MAG(vel);
				Vec2d<int> tile;
				Vec2d<float> d = raycast(pos, vel, tiles, tile);
				d.mag *= cos(heading - atan2(vel.y, vel.x));
				float h = HEIGHT / d.mag;
				h *= (h >= 0);
				SDL_Rect r = { i, (int) ((HEIGHT - h) / 2), 1, (int) h };
				if (r.h > HEIGHT)
				{
					src.y = -r.y * TEXSIZE / (float) r.h;
					src.h -= 2 * src.y;
					if (src.h < 1)
					{
						src.y = TEXSIZE / 2;
						src.h = 1;
					}
					r.y = 0;
					r.h = HEIGHT;
				}
				if (src.h != slice->h)
				{
					SDL_FreeSurface(slice);
					slice = SDL_CreateRGBSurfaceWithFormat(
						textures[0]->flags,
						1, src.h,
						textures[0]->format->BytesPerPixel,
						textures[0]->format->format
					);
				}
				SDL_Surface* t = textures[
					(tiles[tile.y][tile.x] >> tile.mag * 4 & 0xf) - 1
				];
				Uint8 v = 0xff - 0xef * (d.mag / MAXDIST);
				SDL_SetSurfaceColorMod(t, v, v * !(editmode && tile == hl), v);
				src.x = (
					(tile.mag % 2) ?
						pos.x + d.x - tile.x :
						pos.y + d.y - tile.y
				) * TEXSIZE;
				SDL_BlitSurface(t, &src, slice, NULL);
				zoomed = zoomSurface(slice, 1, r.h / (double) src.h, SMOOTHING_ON);
				SDL_BlitSurface(zoomed, NULL, surface, &r);
				SDL_FreeSurface(zoomed);
			}
			SDL_FreeSurface(slice);
		}
		heading = fmod(heading, 2 * M_PI);
		heading += (heading < 0) * 2 * M_PI;
		sprintf(
			infostr,
			"%i deg | %4.1f fps",
			(int) (heading * 180 / M_PI),
			1000 / (float) tdiff
		);
		text = TTF_RenderText_Solid(font, infostr, (SDL_Color) { 255, 255, 255, 255 });
		SDL_Rect rect = { 0, 0, text->w, text->h };
		SDL_BlitSurface(text, NULL, surface, &rect);
		rect = { (WIDTH - ch->w) / 2, (HEIGHT - ch->h) / 2, ch->w, ch->h };
		SDL_BlitSurface(ch, NULL, surface, &rect);
		SDL_UpdateWindowSurface(window);
		SDL_FreeSurface(text);
		// process key event
		if (gotevent && e.type == SDL_KEYDOWN)
		{
			Uint16 current = tiles[hl.y][hl.x];
			Uint16 facetex = current >> hl.mag * 4 & 0xf;
			Uint16 oldtex  = facetex;
			switch (e.key.keysym.sym)
			{
			case SDLK_e:    editmode = !editmode; break;
			case SDLK_DOWN: facetex--; break;
			case SDLK_UP:   facetex++; break;
			case SDLK_o:
				std::ofstream file("maps/maze", std::ios::binary);
				file << MAPVER << std::endl;
				file.write((char*) tiles, MAPH * MAPW * 2);
				file.close();
				break;
			}
			// update the face
			if (editmode && facetex != oldtex)
			{
				facetex = (facetex - 1) % 15 + 1;
				facetex += (facetex <= 0) * 15;
				tiles[hl.y][hl.x] = current & ~(0xf << hl.mag * 4)
				                    | facetex << hl.mag * 4;
			}
		}
	}
	quit(textures, floortex, ceiltex, ch, window, surface);
	return 0;
}
