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
	color obj;
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

class spline
{
protected:
	point *points;
	// row then column
	int pointSize;

public:
	color col;

	spline()
	{
	}

	point calculatePoint(float t)
	{
		point P;
		// Calculate the coefficients of the Bezier basis functions
		float t_powers[4] = {1, t, t * t, t * t * t};

		// Matrix multiplication
		P.x = 0;
		P.y = 0;
		P.z = 0;
		// printf("pointSize: %d\n", pointSize);
		for (int j = 0; j < pointSize; ++j)
		{
			// printf("(%f, %f, %f)\n", points[j].x, points[j].y, points[j].z);
			P.x += points[j].x * t_powers[j];
			P.y += points[j].y * t_powers[j];
			P.z += points[j].z * t_powers[j];
			// printf("(%f, %f, %f)\n", P.x, P.y, P.z);
		}
		return P;
	}

	hit testHit(point p, vec dir)
	{
		hit h;
		for (float i = 0; i < 1; i += 0.001)
		{
			point temp = calculatePoint(i);
			// printf("(%f, %f, %f)\n", temp.x, temp.y, temp.z);
			if (floor(temp.x) == floor(p.x) && floor(temp.y) == floor(p.y))
			{
				// printf("hit: (%f, %f, %f)\n", temp.x, temp.y, temp.z);
				h.hit = true;
				h.dist = dp(p, temp);
				h.P = temp;
				h.obj = col;
				return h;
			}
		}
		// printf("no hit\n");
		h.hit = false;
		return h;
	}
};

class quadraticSpline : public spline
{
public:
	quadraticSpline(point *p, color col)
	{
		points = (point *)calloc(3, sizeof(point));
		pointSize = 3;
		points[0] = p[0];
		points[1] = p[1];
		points[2] = p[2];
		this->col = col;
	}
};

class cubicSpline : spline
{
public:
	cubicSpline(point *p)
	{
		points = (point *)calloc(4, sizeof(point));
		pointSize = 4;
		points[0] = p[0];
		points[1] = p[1];
		points[2] = p[2];
		points[3] = p[3];
		this->col = col;
	}
};

vec Normalise(vec v)
{
	float length = sqrt(dp(v, v));
	v.x /= length;
	v.y /= length;
	v.z /= length;
	return v;
}

void draw(SDL_Renderer *ren, cam c, spline *s, int objCount)
{
	SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
	SDL_RenderClear(ren);
	// sortObjects(objs, &objCount, &c);
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
			// printf("(%f, %f, %f)\n", d.x, d.y, d.z);
			// hit bestHit;
			// bestHit.hit = 0;
			// bestHit.dist = 100000000;
			for (int i = 0; i < objCount; i++)
			{
				hit h = s[i].testHit(o, d);
				if (h.hit)
				{
					// printf("%d\n", i);
					// float normalM = getNormal(h.P, h.obj, d);
					SDL_SetRenderDrawColor(ren,h.obj.r * 255, h.obj.g * 255, h.obj.b * 255, 255);
					SDL_RenderDrawPoint(ren, x + 500, y + 500);
					break;
				}
			}
		}
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
	c.pos.z = -10;
	c.dir.x = 0;
	c.dir.y = 0;
	c.dir.z = 1;
	c.fov = 20;
	c.nearClip = 1;
	c.farClip = 1000000;
	c.orthographic = true;

	int objectCount = 1;

	spline *objs = (spline *)malloc(objectCount * sizeof(spline));
	point p[3];
	p[0].x = 0;
	p[0].y = 0;
	p[0].z = 50;
	p[1].x = 1000;
	p[1].y = 0;
	p[1].z = 0;
	p[2].x = 0;
	p[2].y = 1000;
	p[2].z = 0;
	color c1;
	c1.r = 1;
	c1.g = 0;
	c1.b = 0;
	objs[0] = quadraticSpline(p, c1);


	draw(ren, c, objs, objectCount);
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
				draw(ren, c, objs, objectCount);
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
