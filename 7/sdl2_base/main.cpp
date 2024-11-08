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
	int pointSize;

public:
	color col;
	point *points;

	spline()
	{
	}

	point calculatePoint(float t)
	{
		point P;
		if (pointSize == 3)
		{ // quadratic Bezier curve
			P.x = (1 - t) * (1 - t) * points[0].x + 2 * (1 - t) * t * points[1].x + t * t * points[2].x;
			P.y = (1 - t) * (1 - t) * points[0].y + 2 * (1 - t) * t * points[1].y + t * t * points[2].y;
			P.z = (1 - t) * (1 - t) * points[0].z + 2 * (1 - t) * t * points[1].z + t * t * points[2].z;
		}
		else if (pointSize == 4)
		{ // cubic Bezier curve
			P.x = (1 - t) * (1 - t) * (1 - t) * points[0].x + 3 * (1 - t) * (1 - t) * t * points[1].x + 3 * (1 - t) * t * t * points[2].x + t * t * t * points[3].x;
			P.y = (1 - t) * (1 - t) * (1 - t) * points[0].y + 3 * (1 - t) * (1 - t) * t * points[1].y + 3 * (1 - t) * t * t * points[2].y + t * t * t * points[3].y;
			P.z = (1 - t) * (1 - t) * (1 - t) * points[0].z + 3 * (1 - t) * (1 - t) * t * points[1].z + 3 * (1 - t) * t * t * points[2].z + t * t * t * points[3].z;
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
	void draw(SDL_Renderer *ren)
	{
		SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
		SDL_RenderClear(ren);
		for (float i = 0; i < 1; i += 0.001)
		{
			point p = calculatePoint(i);
			SDL_SetRenderDrawColor(ren, col.r * 255, col.g * 255, col.b * 255, 255);
			SDL_RenderDrawPoint(ren, p.x + 500, p.y + 500);
			// printf("p: %f, %f\n", p.x, p.y);
		}
		for (int i = 0; i < pointSize; i++)
		{
			SDL_Rect rect;
			rect.x = points[i].x + 500 - 10;
			rect.y = points[i].y + 500 - 10;
			rect.w = 20;
			rect.h = 20;
			SDL_SetRenderDrawColor(ren, 255, 255, 0, 255);
			SDL_RenderFillRect(ren, &rect);
		}
		SDL_RenderPresent(ren);
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

class cubicSpline : public spline
{
public:
	cubicSpline(point *p, color col)
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
					SDL_SetRenderDrawColor(ren, h.obj.r * 255, h.obj.g * 255, h.obj.b * 255, 255);
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

	// const int MOVE = 10;
	// cam c;
	// c.pos.x = 0;
	// c.pos.y = 0;
	// c.pos.z = -10;
	// c.dir.x = 0;
	// c.dir.y = 0;
	// c.dir.z = 1;
	// c.fov = 20;
	// c.nearClip = 1;
	// c.farClip = 1000000;
	// c.orthographic = true;

	// int objectCount = 1;

	// spline *objs = (spline *)malloc(objectCount * sizeof(spline));
	point p[4];
	p[0].x = -400;
	p[0].y = 0;
	p[1].x = 400;
	p[1].y = 0;
	p[2].x = 0;
	p[2].y = 400;

	p[3].x = -300;
	p[3].y = 0;
	color c1;
	c1.r = 1;
	c1.g = 0;
	c1.b = 0;
	spline obj = cubicSpline(p, c1);
	obj.draw(ren);
	// draw(ren, c, objs, objectCount);
	char finished = 0;
	// the main event loop
	int selected = -1;
	while (!finished)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_MOUSEBUTTONDOWN:
			{
				if (event.button.button == SDL_BUTTON_LEFT)
				{
					selected = -1;
					for (int i = 0; i < 4; i++)
					{
						if (event.button.x > obj.points[i].x + 500 - 10 && event.button.x < obj.points[i].x + 500 + 10 && event.button.y > obj.points[i].y + 500 - 10 && event.button.y < obj.points[i].y + 500 + 10)
						{
							selected = i;
							break;
						}
					}
				}
			}
			break;
			case SDL_MOUSEBUTTONUP:
			{
				selected = -1;
			}
			break;
			case SDL_MOUSEMOTION:
			{
				obj.points[selected].x = event.motion.x - 500;
				obj.points[selected].y = event.motion.y - 500;
				obj.draw(ren);
			}
			break;
				// case SDL_KEYDOWN:
				// switch (event.key.keysym.sym)
				// {
				// case SDLK_w:
				// 	c.pos.z += MOVE;
				// 	break;
				// case SDLK_a:
				// 	c.pos.x -= MOVE;
				// 	break;
				// case SDLK_s:
				// 	c.pos.z -= MOVE;
				// 	break;
				// case SDLK_d:
				// 	c.pos.x += MOVE;
				// 	break;
				// case SDLK_SPACE:
				// 	c.pos.y -= MOVE;
				// 	break;
				// case SDLK_LSHIFT:
				// 	c.pos.y += MOVE;
				// 	break;
				// case SDLK_ESCAPE:
				// 	finished = 1;
				// 	break;
				// case SDLK_LEFTBRACKET:
				// 	c.fov -= 10;
				// 	break;
				// case SDLK_RIGHTBRACKET:
				// 	c.fov += 10;
				// 	break;
				// case SDLK_p:
				// 	c.orthographic = !c.orthographic;
				// 	break;
				// }
				// draw(ren, c, objs, objectCount);
				// break;

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
