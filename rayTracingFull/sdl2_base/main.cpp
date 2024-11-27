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

	const int MOVE = 10;
	cam *c = new cam(point(0, 0, -100), vec(0, 0, 1), 20, 1, 1000000, 1);
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

	light l;
	l.brightness = 10000;
	l.col.r = 1;
	l.col.g = 1;
	l.col.b = 1;
	l.pos.x = -500;
	l.pos.y = 0;
	l.pos.z = 0;

	printf("Light Generated\n");

	int objectCount = 20;

	sphere objs[objectCount];

	objs[0] = sphere();
	objs[0].pos.x = 0;
	objs[0].pos.y = 0;
	objs[0].pos.z = 20;
	objs[0].m = new mat();
	objs[0].m->ambient.r = 1;
	objs[0].m->ambient.g = 1;
	objs[0].m->ambient.b = 1;
	objs[0].m->diffuse.r = 1;
	objs[0].m->diffuse.g = 0;
	objs[0].m->diffuse.b = 0;
	objs[0].m->specular.r = 1;
	objs[0].m->specular.g = 1;
	objs[0].m->specular.b = 1;
	objs[0].m->roughness = 1;
	objs[0].m->reflectivity = 0;
	objs[0].r = 100;
	objs[0].id = 0;
	objs[1] = sphere();
	objs[1].pos.x = -500;
	objs[1].pos.y = 0;
	objs[1].pos.z = -50;
	objs[1].m->ambient.r = 1;
	objs[1].m->ambient.g = 1;
	objs[1].m->ambient.b = 1;
	objs[1].m->diffuse.r = 1;
	objs[1].m->diffuse.g = 0;
	objs[1].m->diffuse.b = 1;
	objs[1].m->specular.r = 1;
	objs[1].m->specular.g = 1;
	objs[1].m->specular.b = 1;
	objs[1].m->roughness = 1;
	objs[1].m->reflectivity = 0;
	objs[1].r = 50;
	objs[1].id = 1;

	for (int i = 2; i < objectCount; i++)
	{
		objs[i] = sphere();
		objs[i].pos.x = (rand() % 1000) - 500;
		objs[i].pos.y = (rand() % 1000) - 500;
		objs[i].pos.z = (rand() % 200) + 100;
		objs[i].m->ambient.r = rand() / RAND_MAX;
		objs[i].m->ambient.g = rand() / RAND_MAX;
		objs[i].m->ambient.b = rand() / RAND_MAX;
		objs[i].m->diffuse.r = rand() / RAND_MAX;
		objs[i].m->diffuse.g = rand() / RAND_MAX;
		objs[i].m->diffuse.b = rand() / RAND_MAX;
		objs[i].m->specular.r = rand() / RAND_MAX;
		objs[i].m->specular.g = rand() / RAND_MAX;
		objs[i].m->specular.b = rand() / RAND_MAX;
		objs[i].m->roughness = rand() / RAND_MAX;
		objs[i].m->reflectivity = rand() / RAND_MAX;
		objs[i].r = (rand() % 130) + 20;
		objs[i].id = i;
		// printf("(%f, %f, %f)\n", objs[i].col.r, objs[i].col.g, objs[i].col.b);
		// printf("draw %d object\n", i + 1);
	}
	printf("Objects Generated\n");
	c->draw(ren, l, objs, objectCount, 1000, 1000, 1);
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
				case SDLK_w:
					c->pos.z += MOVE;
					break;
				case SDLK_a:
					c->pos.x -= MOVE;
					break;
				case SDLK_s:
					c->pos.z -= MOVE;
					break;
				case SDLK_d:
					c->pos.x += MOVE;
					break;
				case SDLK_SPACE:
					c->pos.y -= MOVE;
					break;
				case SDLK_LSHIFT:
					c->pos.y += MOVE;
					break;
				case SDLK_ESCAPE:
					finished = 1;
					break;
				case SDLK_LEFTBRACKET:
					c->fov -= 10;
					break;
				case SDLK_RIGHTBRACKET:
					c->fov += 10;
					break;
				case SDLK_p:
					c->orthographic = !c->orthographic;
					break;
				}
				c->draw(ren, l, objs, objectCount, 1000, 1000, 1);
				break;

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
