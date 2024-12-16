#include <stdio.h>
#include <math.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <complex.h>

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Window *window = SDL_CreateWindow("Random", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1000, 1000, 0);
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	int px = 500;
	int py = 500;

	while (1)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{

			if (event.type == SDL_MOUSEBUTTONDOWN)
			{
				px = event.motion.x;
				py = event.motion.y;
				for (int x = 0; x < 1000; x++)
				{
					for (int y = 0; y < 1000; y++)
					{
						float d = sqrt((x - px) * (x - px) + (y - py) * (y - py));
						float _Complex thing = 1000 + cexp(2 * M_PI * d * I / 80) / (d * d);
						// printf("%f + %f i absolute %f at (%d, %d)\n", creal(thing) * 255 * 1000000, cimag(thing) * 255 * 1000000, cabs(thing) * 255 * 1000000, x, y);
						SDL_SetRenderDrawColor(renderer, sqrt(cabs(thing)) * 255 > 255 ? 255 : sqrt(cabs(thing)) * 255, 0, 0, 255);
						SDL_RenderDrawPoint(renderer, x, y);
					}
				}
				SDL_RenderPresent(renderer);
			}
			if (event.type == SDL_QUIT)
			{
				SDL_Quit();
				return 0;
			}
		}
	}
}