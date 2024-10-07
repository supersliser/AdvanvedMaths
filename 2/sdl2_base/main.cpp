#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

float f(float x)
{
	return sin(x);
}

float df(float x, float delta)
{
	return (f(x) - f(x + delta)) / delta;
}

float F(float x, float delta)
{
	// Calculate the numerical integration of f(x)
	float sum = 0;
	for (float i = 0; i < x; i += delta)
	{
		sum += f(i) * delta;
	}
	return sum;
}
int main(int argc, char *argv[])
{
	const float delta = 0.01;

	// returns zero on success else non-zero
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		printf("error initializing SDL: %s\n", SDL_GetError());
	}
	SDL_Window *win = SDL_CreateWindow("GAME", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1000, 1000, 0);
	SDL_Renderer *ren = SDL_CreateRenderer(win, -1, 0);
	SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
	SDL_RenderClear(ren);

	// task 1 - red line
	SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
	for (float x = 0; x < 1000; x += 1)
	{
		SDL_RenderDrawPoint(ren, x, (-f(x / 100) * 100) + 500);
	}
	// task 2 - green line
	SDL_SetRenderDrawColor(ren, 0, 255, 0, 255);
	for (float x = 0; x < 1000; x += 1)
	{
		SDL_RenderDrawPoint(ren, x, (-df(x / 100, delta) * 100) + 500);
	}
	// task 3 - blue line
	SDL_SetRenderDrawColor(ren, 0, 0, 255, 255);
	for (float x = 0; x < 1000; x += 1)
	{
		SDL_RenderDrawPoint(ren, x, (-F(x / 100, delta) * 100) + 500);
	}

	SDL_RenderPresent(ren);

	char finished = 0;
	// the main event loop
	while (!finished)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				finished = 1;

				break;
			}
		}
	}

	SDL_Surface *sectionSurface = SDL_CreateRGBSurface(0, 1000, 1000, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
	SDL_RenderReadPixels(ren, NULL, SDL_PIXELFORMAT_RGBA8888, sectionSurface->pixels, sectionSurface->pitch);
	IMG_SaveJPG(sectionSurface, "./box.jpg", 100);

	return 0;
}
