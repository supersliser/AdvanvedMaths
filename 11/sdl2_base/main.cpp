#include <stdio.h>
#include <math.h>
#include <time.h>
#include <SDL2/SDL.h>


int middleSquare(int x)
{
    int squared = x * x;
    int squared_str_len = snprintf(NULL, 0, "%d", squared);
    char *squared_str = (char *)malloc(squared_str_len + 1);
    snprintf(squared_str, squared_str_len + 1, "%d", squared);

    int start = (squared_str_len - 2) / 2;
    int end = start + 2;

    int middle_two_digits = atoi(squared_str + start) % 100;

    free(squared_str);

	printf("%d\n", middle_two_digits);

    return middle_two_digits;
}

int lcg(int a, int m, int c, int x) {
	int out = (a * x + c) % m;

	printf("%d\n", out);
	return out;
}

int bbs(int x, int p1, int p2) {
	int m = p1 * p2;

	int out = (x * x) % m;
	printf("%d\n", out);
	return out;
}
int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Window *window = SDL_CreateWindow("Random", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1000, 1000, 0);
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	const int start = 0;

	for (int x = start; x < 1000 + start; x++) {
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderDrawPoint(renderer, x - start, 1000 - ((lcg(8121, 134456, 28411, x)) % 1000));
		// SDL_RenderDrawPoint(renderer, x - start, 1000 - ((middleSquare(x)) % 1000));
		// SDL_RenderDrawPoint(renderer, x - start, 1000 - ((bbs(x, 107, 191)) % 1000));

	}

	SDL_RenderPresent(renderer);

	while (1)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				SDL_Quit();
				return 0;
			}
		}
	}
}