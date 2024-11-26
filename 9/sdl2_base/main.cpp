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

	vec toVec()
	{
		vec o;
		o.x = r;
		o.y = g;
		o.z = b;
		return o;
	}

	void toCol(vec i)
	{
		r = i.x;
		g = i.y;
		b = i.z;
	}

	void normaliseF()
	{
		if (r > 1)
		{
			r = 1;
		}
		if (r < 0)
		{
			r = 0;
		}
		if (g > 1)
		{
			g = 1;
		}
		if (g < 0)
		{
			g = 0;
		}
		if (b > 1)
		{
			b = 1;
		}
		if (b < 0)
		{
			b = 0;
		}
	}

	void normalise255()
	{
		if (r > 255)
		{
			r = 255;
		}
		if (r < 0)
		{
			r = 0;
		}
		if (g > 255)
		{
			g = 255;
		}
		if (g < 0)
		{
			g = 0;
		}
		if (b > 255)
		{
			b = 255;
		}
		if (b < 0)
		{
			b = 0;
		}
	}
} color;
typedef struct sphere
{
	point pos;
	float r;
	color col;
	int id;
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

color shade(cam c, light l, hit h, float roughness, float reflectivityStrength, sphere *objs, int objCount, int recursionCount, int recursionMax);

bool shadow(cam c, light l, hit h, sphere *sphereObjs, int sphereCount);

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

vec getNormal(point P, sphere obj)
{
	vec N;
	N.x = -(P.x - obj.pos.x);
	N.y = -(P.y - obj.pos.y);
	N.z = -(P.z - obj.pos.z);
	N = Normalise(N);
	return N;
}

vec cp(vec a, vec b)
{
	vec result;
	result.x = a.y * b.z - a.z * b.y;
	result.y = a.z * b.x - a.x * b.z;
	result.z = a.x * b.y - a.y * b.x;
	return result;
}

color reflect(sphere current, point p, vec normal, vec i, sphere *objs, int objCount, light l, int recursionCount, int recursionMax, float reflectivity, float roughness)
{
	vec r;
	r.x = i.x - 2 * (dp(i, normal) * normal.x);
	r.y = i.y - 2 * (dp(i, normal) * normal.y);
	r.z = i.z - 2 * (dp(i, normal) * normal.z);

	for (int i = objCount; i > 0; i--)
	{
		hit h = traceObj(p, r, objs[i]);
		if (h.hit && h.obj.id != current.id)
		{
			// printf("object\n");
			cam newC;
			newC.dir = r;
			newC.pos = p;
			bool shad = shadow(newC, l, h, objs, objCount);
			color s;
			if (shad)
			{
				s.r = 0;
				s.g = 0;
				s.b = 0;
			}
			else
			{
				recursionCount++;
				s = shade(newC, l, h, roughness, reflectivity, objs, objCount, recursionCount, recursionMax);
			}
			return s;
		}
	}
	color o;
	o.r = -1;
	o.g = -1;
	o.b = -1;
	return o;
}

bool shadow(cam c, light l, hit h, sphere *sphereObjs, int sphereCount)
{
	vec lDir;
	lDir.x = l.pos.x - h.P.x;
	lDir.y = l.pos.y - h.P.y;
	lDir.z = l.pos.z - h.P.z;
	lDir = Normalise(lDir);
	for (int i = 0; i < sphereCount; i++)
	{
		hit temp = traceObj(h.P, lDir, sphereObjs[i]);
		if (temp.hit == 1 && temp.dist < h.dist && temp.obj.pos.x != h.obj.pos.x && temp.obj.pos.y != h.obj.pos.y && temp.obj.pos.z != h.obj.pos.z)
		{
			return 1;
		}
	}
	return 0;
}

color shade(cam c, light l, hit h, float roughness, float reflectivityStrength, sphere *objs, int objCount, int recursionCount, int recursionMax)
{
	// l.pos.x = c.pos.x;
	// l.pos.y = c.pos.y;
	// l.pos.z = c.pos.z;
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
	// incident.toCol(Normalise(incident.toVec()));
	if (incident.r > 255)
	{
		incident.r = 255;
	}
	if (incident.g > 255)
	{
		incident.g = 255;
	}
	if (incident.b > 255)
	{
		incident.b = 255;
	}
	// printf("(%f, %f, %f)\n", incident.r, incident.g, incident.b);
	vec H;
	H.x = (c.dir.x + lDir.x) / 2;
	H.y = (c.dir.y + lDir.y) / 2;
	H.z = (c.dir.z + lDir.z) / 2;
	// H.x = 0.5;
	// H.y = 0.5;
	// H.z = 0.5;
	color reflectivity;
	reflectivity.r = pow(dp(getNormal(h.P, h.obj), H), 1 / roughness) * h.obj.col.r;
	reflectivity.g = pow(dp(getNormal(h.P, h.obj), H), 1 / roughness) * h.obj.col.g;
	reflectivity.b = pow(dp(getNormal(h.P, h.obj), H), 1 / roughness) * h.obj.col.b;
	// printf("(%f, %f, %f)\n", reflectivity.r, reflectivity.g, reflectivity.b);
	color reflectedLight;
	reflectedLight.toCol(cp(incident.toVec(), reflectivity.toVec()));
	reflectedLight.r = incident.r * reflectivity.r;
	reflectedLight.g = incident.g * reflectivity.g;
	reflectedLight.b = incident.b * reflectivity.b;
	reflectedLight.normalise255();

	if (recursionCount < recursionMax)
	{
		color reflectedRayColor = reflect(h.obj, c.pos, getNormal(h.P, h.obj), c.dir, objs, objCount, l, recursionCount, recursionMax, reflectivityStrength, roughness);
		if (reflectedRayColor.r != -1)
		{
			color o;
			o.r = reflectedLight.r * (1 - reflectivityStrength) + reflectedRayColor.r * reflectivityStrength;
			o.g = reflectedLight.g * (1 - reflectivityStrength) + reflectedRayColor.g * reflectivityStrength;
			o.b = reflectedLight.b * (1 - reflectivityStrength) + reflectedRayColor.b * reflectivityStrength;

			return o;
		}
	}

	return reflectedLight;
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
							bool shad = shadow(newC, l, h, objs, objCount);
							color s;
							if (shad)
							{
								s.r = 0;
								s.g = 0;
								s.b = 0;
							}
							else
							{
								s = shade(newC, l, h, 1, 0.2, objs, objCount, 0, 2);
							}
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
					s.r += output[(int)float(xx)][(int)float(yy)].r;
					s.g += output[(int)float(xx)][(int)float(yy)].g;
					s.b += output[(int)float(xx)][(int)float(yy)].b;
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
