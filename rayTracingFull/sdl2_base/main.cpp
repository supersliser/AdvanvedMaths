#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include "cam.h"
#include "light.h"
#include "hit.h"
#include "sphere.h"
#include "color.h"
#include "vec.h"
#include "mat.h"
#include "point.h"
#include <math.h>

void sortObjects(sphere *objs, int *objCount, cam *c)
{
	int *toDelete = (int *)calloc(0, sizeof(int));
	int toDeleteSize = 0;
	for (int i = 0; i < *objCount; i++)
	{
		if (objs[i].pos.z - objs[i].r < (*c).pos.z + (*c).nearClip && objs[i].pos.z - objs[i].r > (*c).pos.z + (*c).farClip)
		{
			toDeleteSize++;
			toDelete = (int *)realloc(toDelete, toDeleteSize * sizeof(int));
			toDelete[toDeleteSize - 1] = i;
		}
	}

	for (int i = 0; i < toDeleteSize; i++)
	{
		// printf("%d\n", i);
		fflush(stdout);
		for (int j = 0; j < *objCount; j++)
		{
			objs[i] = objs[j + i];
		}
		*objCount -= 1;
		objs = (sphere *)realloc(objs, *objCount * sizeof(sphere));
	}
	for (int i = 0; i < *objCount; i++)
	{
		for (int j = i + 1; j < *objCount; j++)
		{
			if (objs[i].pos.z < objs[j].pos.z)
			{
				sphere temp = objs[i];
				objs[i] = objs[j];
				objs[j] = temp;
			}
		}
	}
}

void draw(SDL_Renderer *ren, cam c, light l, sphere *objs, int objCount)
{
	const float sampleAmount = 8;
	SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
	SDL_RenderClear(ren);
	sortObjects(objs, &objCount, &c);
	for (int x = -500; x < 500; x++)
	{
		for (int y = -500; y < 500; y++)
		{
			color output[(int)sampleAmount][(int)sampleAmount];
			for (float xx = 0; xx < 1; xx += 1 / sampleAmount)
			{
				for (float yy = 0; yy < 1; yy += 1 / sampleAmount)
				{
					// printf("(%f, %f)\n", xx, yy);
					point o;

					o.x = x + c.pos.x + xx;
					o.y = y + c.pos.y + yy;
					o.z = c.pos.z;
					vec d;

					if (c.orthographic)
					{
						d.x = 0;
						d.y = 0;
						d.z = 1;
					}
					else
					{
						d.x = (x + c.dir.x) * (c.fov / 90);
						d.y = (y + c.dir.y) * (c.fov / 90);
						d.z = (500 + c.dir.z) * (c.fov / 90);
						d = Normalise(d);
					}
					for (int i = objCount; i > 0; i--)
					{
						hit h = traceObj(o, d, objs[i]);
						if (h.hit)
						{
							// printf("object\n");
							cam newC;
							newC.dir = d;
							newC.pos = o;
							color s = h.obj.mat.shade(newC, l, h, objs, objCount, 0, 2);
							// printf("(%f, %f, %f)\n", s.r, s.g, s.b);
							output[(int)float(xx)][(int)float(yy)] = s;

							break;
							// printf("(%f, %f, %f)\n", s.r, s.g, s.b);
						}
					}
				}
			}
			color s;
			for (float xx = 0; xx < 1; xx += 1 / sampleAmount)
			{
				for (float yy = 0; yy < 1; yy += 1 / sampleAmount)
				{
					vec temp = s.toVec();

					temp.add(output[(int)float(xx)][(int)float(yy)]);
					s = temp;
					s.normalise255();
				}

			}
			s.r /= sampleAmount * sampleAmount;
			s.g /= sampleAmount * sampleAmount;
			s.b /= sampleAmount * sampleAmount;
			s.normalise255();
			SDL_SetRenderDrawColor(ren, floor(s.r), floor(s.g), floor(s.b), 255);
			printf("(%f, %f, %f)\n at (%d, %d)\n", s.r, s.g, s.b, x, y);
			SDL_RenderDrawPoint(ren, x + 500, y + 500);
		}
		// SDL_RenderPresent(ren);
	}
	printf("drawn");
	fflush(stdout);
	SDL_RenderPresent(ren);
	// printf("drawn");
}
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

	const int MOVE = 10;
	cam c;
	c.pos.x = 0;
	c.pos.y = 0;
	c.pos.z = -100;
	c.dir.x = 0;
	c.dir.y = 0;
	c.dir.z = 1;
	c.fov = 20;
	c.nearClip = 1;
	c.farClip = 1000000;
	c.orthographic = 1;

	light l;
	l.brightness = 10000;
	l.col.r = 1;
	l.col.g = 1;
	l.col.b = 1;
	l.pos.x = -500;
	l.pos.y = 0;
	l.pos.z = 0;

	int objectCount = 20;

	sphere *temp = (sphere *)malloc(sizeof(sphere) * objectCount);

	temp[0].pos.x = 0;
	temp[0].pos.y = 0;
	temp[0].pos.z = 20;
	temp[0].col.r = 1;
	temp[0].col.g = 0;
	temp[0].col.b = 0;
	temp[0].r = 100;
	temp[0].id = 0;
	temp[1].pos.x = -500;
	temp[1].pos.y = 0;
	temp[1].pos.z = -50;
	temp[1].col.r = 1;
	temp[1].col.g = 0;
	temp[1].col.b = 1;
	temp[1].r = 50;
	temp[1].id = 1;

	for (int i = 2; i < objectCount; i++)
	{
		temp[i].pos.x = (rand() % 1000) - 500;
		temp[i].pos.y = (rand() % 1000) - 500;
		temp[i].pos.z = (rand() % 200) + 100;
		temp[i].col.r = ((float)(rand() % 155) + 100) / 255;
		temp[i].col.g = ((float)(rand() % 155) + 100) / 255;
		temp[i].col.b = ((float)(rand() % 155) + 100) / 255;
		temp[i].r = (rand() % 130) + 20;
		temp[i].id = i;
		// printf("(%f, %f, %f)\n", objs[i].col.r, objs[i].col.g, objs[i].col.b);
		// printf("draw %d object\n", i + 1);
	}
	sphere **objs = &temp;
	draw(ren, c, l, *objs, objectCount);
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
					c.pos.z += MOVE;
					break;
				case SDLK_a:
					c.pos.x -= MOVE;
					break;
				case SDLK_s:
					c.pos.z -= MOVE;
					break;
				case SDLK_d:
					c.pos.x += MOVE;
					break;
				case SDLK_SPACE:
					c.pos.y -= MOVE;
					break;
				case SDLK_LSHIFT:
					c.pos.y += MOVE;
					break;
				case SDLK_ESCAPE:
					finished = 1;
					break;
				case SDLK_LEFTBRACKET:
					c.fov -= 10;
					break;
				case SDLK_RIGHTBRACKET:
					c.fov += 10;
					break;
				case SDLK_p:
					c.orthographic = !c.orthographic;
					break;
				}
				draw(ren, c, l, *objs, objectCount);
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
