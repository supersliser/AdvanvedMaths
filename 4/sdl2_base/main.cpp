#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

typedef struct Point
{
	float x;
	float y;
	float z;
};

Point sphere(float u, float v, float ax, float az)
{
	const float r = 1000;
	float dr = r * cos(M_PI * (v - 0.5));

	ax = ax * M_PI / 180;
	az = az * M_PI / 180;

	Point p;
	p.x = dr * cos(2 * M_PI * u);
	p.y = dr * sin(2 * M_PI * u);
	p.z = r * sin(M_PI * (v - 0.5));

	p.y = p.y * cos(ax) - p.z * sin(ax);
	p.z = p.y * sin(ax) + p.z * cos(ax);
	p.x = p.x * cos(az) - p.y * sin(az);
    p.y = p.x * sin(az) + p.y * cos(az);
	return p;
}

Point Cylinder(float u, float v, float ax, float az)
{
	const float r = 100;
	const float h = 1000;

	ax = ax * M_PI / 180;
	az = az * M_PI / 180;

	Point p;
	p.x = r * cos(2 * M_PI * u);
	p.y = r * sin(2 * M_PI * u);
	p.z = v * h;

	p.y = p.y * cos(ax) - p.z * sin(ax);
	p.z = p.y * sin(ax) + p.z * cos(ax);
	p.x = p.x * cos(az) - p.y * sin(az);
    p.y = p.x * sin(az) + p.y * cos(az);
	return p;
}

Point Cone(float u, float v, float ax, float az)
{
	const float r = 100;
	const float h = 1000;

	ax = ax * M_PI / 180;
	az = az * M_PI / 180;

	Point p;
	p.x = (1 - v) * r * cos(2 * M_PI * u);
	p.y = (1 - v) * r * sin(2 * M_PI * u);
	p.z = v * h;

	p.y = p.y * cos(ax) - p.z * sin(ax);
	p.z = p.y * sin(ax) + p.z * cos(ax);
	p.x = p.x * cos(az) - p.y * sin(az);
    p.y = p.x * sin(az) + p.y * cos(az);
	return p;
}

void Draw(SDL_Renderer *ren, float ax, float az)
{
	const float uRes = 0.05;
	const float vRes = 0.05;
	for (float u = 0; u < 1; u += uRes)
	{
		for (float v = 0; v < 1; v += vRes)
		{
			Point c;
			c.x = 0;
			c.y = 0;
			c.z = -2000;
			Point p = Cone(u, v, ax, az);
			SDL_RenderDrawPointF(ren, (p.x / (p.z + c.z)) * 500 + 500 + c.x, (p.y / (p.z + c.z)) * 500 + 500 + c.y);
		}
	}
}

int main(int argc, char *argv[])
{

	// returns zero on success else non-zero
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		printf("error initializing SDL: %s\n", SDL_GetError());
	}
	SDL_Window *win = SDL_CreateWindow("4",
									   SDL_WINDOWPOS_CENTERED,
									   SDL_WINDOWPOS_CENTERED,
									   1000, 1000, 0);

	SDL_Renderer *ren = SDL_CreateRenderer(win, -1, 0);
	SDL_RenderClear(ren);
	SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);

	float ax = 0;
	float az = 0;

	Draw(ren, ax, az);

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
			case SDL_KEYDOWN:

				switch (event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					finished = 1;
					break;
				case SDLK_w:
					SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
					SDL_RenderClear(ren);
					SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
					ax += 5;
					Draw(ren, ax, az);
					break;
				case SDLK_s:
					SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
					SDL_RenderClear(ren);
					SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
					ax -= 5;
					Draw(ren, ax, az);
					break;
									case SDLK_a:
					SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
					SDL_RenderClear(ren);
					SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
					az += 5;
					Draw(ren, ax, az);
					break;
				case SDLK_d:
					SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
					SDL_RenderClear(ren);
					SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
					az -= 5;
					Draw(ren, ax, az);
					break;
				}
				SDL_RenderPresent(ren);
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
