#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

typedef struct point
{
	float x;
	float y;
	float z;
};
typedef struct vec
{
	float x;
	float y;
	float z;
};
typedef struct color
{
	float r;
	float g;
	float b;
};
typedef struct sphere
{
	point pos;
	float r;
	color col;
};
typedef struct hit
{
	char hit;
	sphere obj;
	float dist;
	point P;
};

float dp(point p1, vec p2) {
	return p1.x * p2.x + p1.y * p2.y + p1.z * p2.z;
}
float dp(point p1, point p2) {
	return p1.x * p2.x + p1.y * p2.y + p1.z * p2.z;
}
float dp(vec p1, point p2) {
	return p1.x * p2.x + p1.y * p2.y + p1.z * p2.z;
}
float dp(vec p1, vec p2) {
	return p1.x * p2.x + p1.y * p2.y + p1.z * p2.z;
}

hit traceObj(point src, vec dir, sphere obj) {
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
	printf("(%f, %f, %f)\n", P.x, P.y, P.z);
	hit out;
	if (isnan(P.x)) {
		out.hit = 0;
	}
	else {
		out.hit = 1;
		out.obj = obj;
		out.dist = t0;
		out.P = P;
	}
	return out;
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
	SDL_RenderClear(ren);

	sphere obj;
	obj.pos.x = 200;
	obj.pos.y = 300;
	obj.pos.z = 0;
	obj.col.r = 1;
	obj.col.g = 0;
	obj.col.b = 0;
	obj.r = 500;

	point c;
	c.x = 0;
	c.y = 0;
	c.z = -40;

	vec d;
	d.x = 0;
	d.y = 0;
	d.z = 1;

	for (int x = 0; x < 1000; x++) {
		for (int y = 0; y < 1000; y++) {
			c.x = x - 500;
			c.y = y - 500;
			hit h = traceObj(c, d, obj);
			vec N;
			N.x = (h.P.x - h.obj.pos.x) / h.obj.r;
			N.y = (h.P.y - h.obj.pos.y) / h.obj.r;
			N.z = (h.P.z - h.obj.pos.z) / h.obj.r;
			vec I;
			I.x = h.P.x-c.x;
			I.y = h.P.y-c.y;
			I.z = h.P.z-c.z;
			vec V;
			float mI = sqrt((I.x * I.x) + (I.y * I.y) + (I.z * I.z));
			V.x = -I.x / mI;
			V.y = -I.y / mI;
			V.z = -I.z / mI;
			N.x = (h.P.x - h.obj.pos.x) / h.obj.r;
			N.y = (h.P.y - h.obj.pos.y) / h.obj.r;
			N.z = (h.P.z - h.obj.pos.z) / h.obj.r;
			
			// Normalize the vector to ensure it's a unit vector
			float norm = sqrt(N.x * N.x + N.y * N.y + N.z * N.z);
			N.x /= norm;
			N.y /= norm;
			N.z /= norm;
			// if (V.x <= 0) {
			// 	V.x = 0;
			// }
			// if (V.y <= 0) {
			// 	V.y = 0;
			// }
			// if (V.z <= 0) {
			// 	V.z = 0;
			// }
			if (h.hit) {
				SDL_SetRenderDrawColor(ren, h.obj.col.r * 255 * dp(N, V), h.obj.col.g * 255 * dp(N, V), h.obj.col.b * 255 * dp(N, V), 255);
				SDL_RenderDrawPoint(ren, x, y);
			}
		}
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
