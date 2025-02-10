#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include "cam.h"
#include "light.h"
#include "mat.h"
#include "sphere.h"
#include "hit.h"
#include "color.h"
#include "vec.h"
#include "point.h"
#include <stdio.h>
#include <math.h>
#include <time.h>

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

	const int samples = 2;
	const int reflectionBounces = 1;
	const int objectCount = 50;
	const SampleType sampleType = RANDOM_SHRINKING_PIXELS;
	const bool progressiveRender = 1;
	const int passes = 25000;
	const bool trueRandom = 1;
	const int pixelSize = 1;
	const int lightCount = 20;
	const int importanceStart = 200;
	const int importanceVarianceSize = 1;
	const int generationEnd = 40;
	const int frameStart = 0;
	const int frameEnd = 1;
	const int MOVE = 20;

	if (trueRandom)
	{
		srandom(time(NULL));
	}

	cam *c = new cam(point(0, 0, -200), vec(0, 0, 1), 70, 1);
	// c->pos.x = 0;
	// c->pos.y = 0;
	// c->pos.z = -100;
	// c->dir.x = 0;
	// c->dir.y = 0;
	// c->dir.z = 1;
	// c->fov = 20;
	// c->nearClip = 1;
	// c->farClip = 1000000;
	// c->orthographic = 1;
	printf("Cam Generated\n");
	light ls[lightCount];

		ls[0].brightness = 99999999999;
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
		ls[i].pos.x = random() % 200 - 100;
		ls[i].pos.y = random() % 200 - 100;
		ls[i].pos.z = random() % 200 - 100;
	}

	printf("Light Generated\n");

	sphere objs[objectCount];

	if (!trueRandom)
	{
		objs[0] = sphere();
		objs[0].addKeyframe(frameStart, point(0, 0, 0));
		objs[0].addKeyframe(frameEnd, point(0, 0, -500));
		objs[0].m = new mat();
		objs[0].m->ambient.r = 0;
		objs[0].m->ambient.g = 0;
		objs[0].m->ambient.b = 0;
		objs[0].m->diffuse.r = 1;
		objs[0].m->diffuse.g = 0;
		objs[0].m->diffuse.b = 0;
		objs[0].m->specular.r = 1;
		objs[0].m->specular.g = 1;
		objs[0].m->specular.b = 1;
		objs[0].m->roughness = 0.2;
		objs[0].m->reflectivity = 1;
		objs[0].r = 100;
		objs[0].id = 0;
		objs[1] = sphere();
		objs[1].addKeyframe(frameStart, point(-50, 0, -500));
		objs[1].addKeyframe(frameEnd, point(50, 0, -500));
		objs[1].m = new mat();
		objs[1].m->ambient.r = 0.2;
		objs[1].m->ambient.g = 0.2;
		objs[1].m->ambient.b = 0.2;
		objs[1].m->diffuse.r = 1;
		objs[1].m->diffuse.g = 0;
		objs[1].m->diffuse.b = 1;
		objs[1].m->specular.r = 1;
		objs[1].m->specular.g = 1;
		objs[1].m->specular.b = 1;
		objs[1].m->roughness = 0.1;
		objs[1].m->reflectivity = 1;
		objs[1].r = 50;
		objs[1].id = 1;
	}
	color col = color(0, 0, 0);
	for (int i = (trueRandom ? 2 : 0); i < objectCount; i++)
	{
		objs[i] = sphere();
		point temp = point((rand() % 1000) - 500, (rand() % 1000) - 500, (rand() % 200));
		objs[i].addKeyframe(frameStart, temp);
		temp.x += (rand() % 2 == 1 ? 1 : -1) * (rand() % MOVE);
		temp.y += (rand() % 2 == 1 ? 1 : -1) * (rand() % MOVE);
		temp.z += rand() % MOVE;
		objs[i].addKeyframe(frameEnd, temp);
		objs[i].m = new mat();
		objs[i].m->ambient.r = 0;
		objs[i].m->ambient.g = 0;
		objs[i].m->ambient.b = 0;
		objs[i].m->diffuse = col.randColor();
		objs[i].m->specular = col.randColor();
		objs[i].m->roughness = (float)rand() / RAND_MAX;
		objs[i].m->reflectivity = (float)rand() / RAND_MAX;
		objs[i].r = (rand() % 90) + 10;
		objs[i].id = i;
		// printf("(%f, %f, %f)\n", objs[i].col.r, objs[i].col.g, objs[i].col.b);
		// printf("draw %d object\n", i + 1);
	}
	printf("Objects Generated\n");
	// c->draw(ren, ls, lightCount, objs, objectCount, 1000, 1000, samples, reflectionBounces, progressiveRender, sampleType, passes, pixelSize, importanceStart, generationEnd, importanceVarianceSize, 0);
	c->renderFrames(ren, ls, lightCount, objs, objectCount, 1000, 1000, samples, reflectionBounces, progressiveRender, sampleType, passes, pixelSize, importanceStart, generationEnd, importanceVarianceSize, frameStart, frameEnd);
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
			case SDL_QUIT:
				finished = 1;
				break;
			}
		}
	}

	// SDL_Surface *sectionSurface = SDL_CreateRGBSurface(0, 1000, 1000, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
	// SDL_RenderReadPixels(ren, NULL, SDL_PIXELFORMAT_RGBA8888, sectionSurface->pixels, sectionSurface->pitch);
	// IMG_SaveJPG(sectionSurface, "./box.jpg", 100);

	return 0;
}
