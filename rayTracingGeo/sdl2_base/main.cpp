#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include "cam.h"
#include "light.h"
#include "mat.h"
#include "hit.h"
#include "color.h"
#include "vec.h"
#include "point.h"
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "geo.h"

int main(int argc, char *argv[])
{

	// returns zero on success else non-zero
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		printf("error initializing SDL: %s\n", SDL_GetError());
	}
	SDL_Window *win = SDL_CreateWindow("GAME",
									   SDL_WINDOWPOS_CENTERED,
									   SDL_WINDOWPOS_CENTERED,
									   1000, 1000, 0);

	SDL_Renderer *ren = SDL_CreateRenderer(win, -1, 0);
	SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
	SDL_RenderClear(ren);

	printf("SDL Initialized\n");
	fflush(stdout);

	const int samples = 1;
	const int reflectionBounces = 0;
	const int objectCount = 1;
	const SampleType sampleType = RANDOM;
	const bool progressiveRender = 1;
	const int passes = 10000;
	const bool trueRandom = 1;
	const int pixelSize = 1;
	const int lightCount = 3;
	const int importanceStart = 200;
	const int importanceVarianceSize = 10;
	const int generationEnd = 40;

	if (trueRandom)
	{
		srandom(time(NULL));
	}

	cam *c = new cam(point(2.5, -2.5, 5), vec(0, 0, -1), 75, 0);

	printf("Cam Generated\n");
	c->printCam(); // Add this line to print camera details

	light ls[lightCount];

	ls[0].brightness = MAXFLOAT;
	ls[0].col.r = 1;
	ls[0].col.g = 1;
	ls[0].col.b = 1;
	ls[0].pos.x = 0;
	ls[0].pos.y = 0;
	ls[0].pos.z = 0;

	for (int i = 1; i < lightCount; i++)
	{
		ls[i].brightness = MAXFLOAT;
		ls[i].col.r = 1;
		ls[i].col.g = 1;
		ls[i].col.b = 1;
		ls[i].pos.x = random() / (double)RAND_MAX * 200000 - 100000;
		ls[i].pos.y = random() / (double)RAND_MAX * 200000 - 100000;
		ls[i].pos.z = random() / (double)RAND_MAX * 200000 - 100000;
	}

	printf("Light Generated\n");

	geo objs[objectCount];
	objs[0] = geo(point(0, 0, 0), new mat(color(1, 1, 1), color(1, 0, 0), color(1, 1, 1), 0.2, 1));
	objs[0].sphere(1, 9, 9);
	// objs[1] = geo(point(3, -1, 0), new mat(color(1, 1, 1), color(1, 0, 0), color(1, 1, 1), 0.2, 1));
	// objs[1].cube(1);

	printf("Objects Generated\n");

	c->draw(ren, ls, lightCount, objs, objectCount, 1000, 1000, samples, reflectionBounces, progressiveRender, sampleType, passes, pixelSize, importanceStart, generationEnd, importanceVarianceSize);

	char finished = 0;
	// the main event loop
	while (!finished)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					finished = 1;
					break;
				}
				break;

			case SDL_QUIT:
				finished = 1;
				break;
			}
		}
	}

	// SDL_Surface *sectionSurface = SDL_CreateRGBSurface(0, 1000, 1000, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
	// SDL_RenderReadPixels(ren, NULL, SDL_PIXELFORMAT_RGBA8888, sectionSurface->pixels, sectionSurface->pitch);
	// IMG_SaveJPG(sectionSurface, "./box.jpg", 100);

	// Clean up
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();

	delete c;

	return 0;
}
