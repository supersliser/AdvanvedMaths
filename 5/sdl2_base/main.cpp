#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>

typedef struct point
{
	float x;
	float y;
	float z;
} point;
typedef struct vec
{
	float x;
	float y;
	float z;
} vec;
typedef struct color
{
	float r;
	float g;
	float b;
} color;
typedef struct sphere
{
	point pos;
	float r;
	color col;
} sphere;
typedef struct hit
{
	char hit;
	sphere obj;
	float dist;
	point P;
} hit;
typedef struct cam
{
	point pos;
	vec dir;
} cam;

float dp(point p1, vec p2)
{
	return p1.x * p2.x + p1.y * p2.y + p1.z * p2.z;
}
float dp(point p1, point p2)
{
	return p1.x * p2.x + p1.y * p2.y + p1.z * p2.z;
}
float dp(vec p1, point p2)
{
	return p1.x * p2.x + p1.y * p2.y + p1.z * p2.z;
}
float dp(vec p1, vec p2)
{
	return p1.x * p2.x + p1.y * p2.y + p1.z * p2.z;
}

void sortObjects(sphere *objs, int objCount)
{
	for (int i = 0; i < objCount; i++)
	{
		for (int j = i + 1; j < objCount; j++)
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

hit traceObj(point src, vec dir, sphere obj)
{
	hit out;
	// if (obj.pos.z < src.z) {
	// 	out.hit = 0;
	// 	return out;
	// }
	point O = src;
	vec D = dir;
	point C = obj.pos;
	float r = obj.r;
	point L;
	L.x = C.x - O.x;
	L.y = C.y - O.y;
	L.z = C.z - O.z;
	float tca = dp(L, D);
	float d = sqrt(dp(L, L) - (tca * tca));
	float thc = sqrt((r * r) - (d * d));
	float t0 = tca - thc;
	vec tD;
	tD.x = D.x * t0;
	tD.y = D.y * t0;
	tD.z = D.z * t0;
	point P;
	P.x = O.x + tD.x;
	P.y = O.y + tD.y;
	P.z = O.z + tD.z;
	// printf("(%f, %f, %f)\n", P.x, P.y, P.z);
	if (isnan(P.x))
	{
		out.hit = 0;
	}
	else
	{
		out.hit = 1;
		out.obj = obj;
		out.dist = t0;
		out.P = P;
	}
	return out;
}

vec getNormal(point P, sphere obj)
{
	vec N;
	N.x = (P.x - obj.pos.x);
	N.y = (P.y - obj.pos.y);
	N.z = (P.z - obj.pos.z);
	float length = sqrt(N.x * N.x + N.y * N.y + N.z * N.z);
	N.x /= length;
	N.y /= length;
	N.z /= length;
	return N;
}

void draw(SDL_Renderer *ren, cam c, sphere *objs, int objCount)
{
	SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
	SDL_RenderClear(ren);
	for (int x = 0; x < 1000; x++)
	{
		for (int y = 0; y < 1000; y++)
		{
			point o;
			o.x = x + ((c.pos.x - 500) * 2);
			o.y = y + ((c.pos.y - 500) * 2);
			o.z = c.pos.z;
			hit bestHit;
			bestHit.hit = 0;
			bestHit.dist = 1000000000;
			for (int i = 0; i < objCount; i++)
			{
				hit h = traceObj(o, c.dir, objs[i]);
				if (h.hit)
				{
					if (h.dist < bestHit.dist)
					{
						bestHit = h;
					}
				}
			}
			if (bestHit.hit){
					vec normal = getNormal(bestHit.P, bestHit.obj);
					vec viewDir;
					viewDir.x = c.pos.x - bestHit.P.x;
					viewDir.y = c.pos.y - bestHit.P.y;
					viewDir.z = c.pos.z - bestHit.P.z;
					float length = sqrt(viewDir.x * viewDir.x + viewDir.y * viewDir.y + viewDir.z * viewDir.z);
					viewDir.x /= length;
					viewDir.y /= length;
					viewDir.z /= length;
					float dotProduct = normal.x * viewDir.x + normal.y * viewDir.y + normal.z * viewDir.z;
					SDL_SetRenderDrawColor(ren, bestHit.obj.col.r * 255 * dotProduct, bestHit.obj.col.g * 255 * dotProduct, bestHit.obj.col.b * 255 * dotProduct, 255);
					SDL_RenderDrawPoint(ren, x, y);
			}
		}
	}
			SDL_RenderPresent(ren);
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
	SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
	SDL_RenderClear(ren);

	const int MOVE = 10;
	cam c;
	c.pos.x = 200;
	c.pos.y = 300;
	c.pos.z = -1000;
	c.dir.x = 0;
	c.dir.y = 0;
	c.dir.z = 1;

	sphere *objs = (sphere *)malloc(sizeof(sphere) * 4);
	objs[0].pos.x = 300;
	objs[0].pos.y = 200;
	objs[0].pos.z = 0;
	objs[0].col.r = 1;
	objs[0].col.g = 0;
	objs[0].col.b = 1;
	objs[0].r = 100;
	objs[1].pos.x = -300;
	objs[1].pos.y = 0;
	objs[1].pos.z = 0;
	objs[1].col.r = 1;
	objs[1].col.g = 0;
	objs[1].col.b = 1;
	objs[1].r = 100;
	objs[2].pos.x = 300;
	objs[2].pos.y = 200;
	objs[2].pos.z = 0;
	objs[2].col.r = 1;
	objs[2].col.g = 0;
	objs[2].col.b = 0;
	objs[2].r = 25;
	objs[3].pos.x = -300;
	objs[3].pos.y = 0;
	objs[3].pos.z = -50;
	objs[3].col.r = 1;
	objs[3].col.g = 0.5;
	objs[3].col.b = 1;
	objs[3].r = 300;

	sortObjects(objs, 4);

	draw(ren, c, objs, 4);
	// for (int i = 1; i < 20; i++){
	// 	objs[i].pos.x = (rand() * 500) - 500;
	// 	objs[i].pos.y = (rand() * 500) - 500;
	// 	objs[i].pos.z = (rand() * 500) - 500;
	// 	objs[i].col.r = 1;
	// 	objs[i].col.g = 0;
	// 	objs[i].col.b = 1;
	// 	objs[i].r = 100;
	// 	draw(ren, c, objs, i+1);
	// 		printf("draw %d object\n", i+1);
	// }

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
					c.pos.z += MOVE * 10;
					break;
				case SDLK_a:
					c.pos.x -= MOVE;
					break;
				case SDLK_s:
					c.pos.z -= MOVE * 10;
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
				}
				draw(ren, c, objs, 4);
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
