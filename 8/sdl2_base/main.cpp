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
	float fov;
	float nearClip;
	float farClip;
	bool orthographic;
} cam;

typedef struct light
{
	point pos;
	color col;
	float brightness;
} light;

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

vec Normalise(vec v)
{
	float length = sqrt(dp(v, v));
	v.x /= length;
	v.y /= length;
	v.z /= length;
	return v;
}

vec getNormal(point P, sphere obj, vec cDir)
{
	vec N;
	N.x = (obj.pos.x - P.x) / 1000;
	N.y = (obj.pos.x - P.y) / 1000;
	N.z = (obj.pos.z - P.z) / 1000;
	return N;
}

color cp(color a, color b)
{
	color result;
	result.r = a.g * b.b - a.b * b.g;
	result.g = a.b * b.r - a.r * b.b;
	result.b = a.r * b.g - a.g * b.r;
	return result;
}

color shade(cam c, light l, hit h, float roughness)
{
	l.pos.x = c.pos.x;
	l.pos.y = c.pos.y;
	l.pos.z = c.pos.z;
	vec lDir;
	lDir.x = l.pos.x - h.P.x;
	lDir.y = l.pos.y - h.P.y;
	lDir.z = l.pos.z - h.P.z;
	lDir = Normalise(lDir);
	color incident;
	float dpl = dp(lDir, lDir);
	incident.r = l.col.r * l.brightness / dpl;
	incident.g = l.col.g * l.brightness / dpl;
	incident.b = l.col.b * l.brightness / dpl;
	vec H;
	// H.x = (c.dir.x + lDir.x) / 2;
	// H.y = (c.dir.y + lDir.y) / 2;
	// H.z = (c.dir.z + lDir.z) / 2;
	H.x = 0.5;
	H.y = 0.5;
	H.z = 0.5;
	color reflectivity;
	reflectivity.r = pow(dp(getNormal(h.P, h.obj, c.dir), H), 1 / roughness) * h.obj.col.r;
	reflectivity.g = pow(dp(getNormal(h.P, h.obj, c.dir), H), 1 / roughness) * h.obj.col.g;
	reflectivity.b = pow(dp(getNormal(h.P, h.obj, c.dir), H), 1 / roughness) * h.obj.col.b;
	// printf("(%f, %f, %f)\n", reflectivity.r, reflectivity.g, reflectivity.b);
	color reflectedLight = cp(incident, reflectivity);
	return reflectedLight;
}

void draw(SDL_Renderer *ren, cam c, light l, sphere *objs, int objCount)
{
	SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
	SDL_RenderClear(ren);
	sortObjects(objs, &objCount, &c);
	for (int x = -500; x < 500; x++)
	{
		for (int y = -500; y < 500; y++)
		{
			point o;

			o.x = x + c.pos.x;
			o.y = y + c.pos.y;
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
					color s = shade(newC, l, h, 1);
					printf("(%f, %f, %f)\n", s.r, s.g, s.b);
					SDL_SetRenderDrawColor(ren, floor(s.r * 255), floor(s.g * 255), floor(s.b * 255), 255);
					SDL_RenderDrawPoint(ren, x + 500, y + 500);
					break;
				}
			}
		}
	}
	// printf("drawn");
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
	c.pos.z = -200;
	c.dir.x = 0;
	c.dir.y = 0;
	c.dir.z = 1;
	c.fov = 20;
	c.nearClip = 1;
	c.farClip = 1000000;
	c.orthographic = false;

	light l;
	l.brightness = 1;
	l.col.r = 1;
	l.col.g = 0;
	l.col.b = 0;
	l.pos.x = 100;
	l.pos.y = 100;
	l.pos.z = 100;

	int objectCount = 2;

	sphere *temp = (sphere *)malloc(sizeof(sphere) * objectCount);

	temp[0].pos.x = 0;
	temp[0].pos.y = 0;
	temp[0].pos.z = 100;
	temp[0].col.r = 1;
	temp[0].col.g = 0;
	temp[0].col.b = 0;
	temp[0].r = 200;
	temp[1].pos.x = -200;
	temp[1].pos.y = 0;
	temp[1].pos.z = 100;
	temp[1].col.r = 1;
	temp[1].col.g = 0;
	temp[1].col.b = 1;
	temp[1].r = 100;

	// for (int i = 2; i < objectCount; i++)
	// {
	// 	temp[i].pos.x = (rand() % 2000) - 500;
	// 	temp[i].pos.y = (rand() % 2000) - 500;
	// 	temp[i].pos.z = (rand() % 2000) - 1000;
	// 	temp[i].col.r = ((float)(rand() % 155) + 100) / 255;
	// 	temp[i].col.g = ((float)(rand() % 155) + 100) / 255;
	// 	temp[i].col.b = ((float)(rand() % 155) + 100) / 255;
	// 	temp[i].r = (rand() % 100) + 50;
	// 	// printf("(%f, %f, %f)\n", objs[i].col.r, objs[i].col.g, objs[i].col.b);
	// 	// printf("draw %d object\n", i + 1);
	// }
	sphere **objs = &temp;
	draw(ren, c,l, *objs, objectCount);
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
				draw(ren, c,l, *objs, objectCount);
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