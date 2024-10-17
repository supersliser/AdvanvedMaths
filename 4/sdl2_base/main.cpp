#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

typedef struct Point
{
	float x;
	float y;
	float z;
};

Point sphere(float u, float v, float ax, float az, float ay)
{
	const float r = 1000;
	float dr = r * cos(M_PI * (v - 0.5));

	ax = ax * M_PI / 180;
	az = az * M_PI / 180;
	ay = ay * M_PI / 180;

	Point p;
	p.x = dr * cos(2 * M_PI * u);
	p.y = dr * sin(2 * M_PI * u);
	p.z = r * sin(M_PI * (v - 0.5));

	p.y = p.y * cos(ax) - p.z * sin(ax);
	p.z = p.y * sin(ax) + p.z * cos(ax);
	p.x = p.x * cos(az) - p.y * sin(az);
	p.y = p.x * sin(az) + p.y * cos(az);
	p.z = p.z * cos(ay) - p.x * sin(ay);
	p.x = p.z * sin(ay) + p.x * cos(ay);
	return p;
}

Point Cylinder(float u, float v, float ax, float az, float ay)
{
	const float r = 100;
	const float h = 1000;

	ax = ax * M_PI / 180;
	az = az * M_PI / 180;
	ay = ay * M_PI / 180;

	Point p;
	p.x = r * cos(2 * M_PI * u);
	p.y = r * sin(2 * M_PI * u);
	p.z = v * h;

	p.y = p.y * cos(ax) - p.z * sin(ax);
	p.z = p.y * sin(ax) + p.z * cos(ax);
	p.x = p.x * cos(az) - p.y * sin(az);
	p.y = p.x * sin(az) + p.y * cos(az);
	p.z = p.z * cos(ay) - p.x * sin(ay);
	p.x = p.z * sin(ay) + p.x * cos(ay);
	return p;
}

Point Cone(float u, float v, float ax, float az, float ay)
{
	const float r = 100;
	const float h = 1000;

	ax = ax * M_PI / 180;
	az = az * M_PI / 180;
	ay = ay * M_PI / 180;

	Point p;
	p.x = (1 - v) * r * cos(2 * M_PI * u);
	p.y = (1 - v) * r * sin(2 * M_PI * u);
	p.z = v * h;

	p.y = p.y * cos(ax) - p.z * sin(ax);
	p.z = p.y * sin(ax) + p.z * cos(ax);
	p.x = p.x * cos(az) - p.y * sin(az);
	p.y = p.x * sin(az) + p.y * cos(az);
	p.z = p.z * cos(ay) - p.x * sin(ay);
	p.x = p.z * sin(ay) + p.x * cos(ay);
	return p;
}

void Draw(SDL_Renderer *ren, Point cp, char obj, Point cr)
{
	SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
	SDL_RenderClear(ren);
	SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
	const float uRes = 0.05;
	const float vRes = 0.05;

	cp.x += cr.x;
	cp.y += cr.y;
	cp.z += cr.z;
	for (float u = 0; u < 1; u += uRes)
	{
		for (float v = 0; v < 1; v += vRes)
		{
			Point p;
			switch (obj)
			{
			case 0:
				p = Cone(u, v, 0, 0, 0);
				break;
			case 1:
				p = sphere(u, v, 0, 0, 0);
				break;
			case 2:
				p = Cylinder(u, v, 0, 0, 0);
				break;
			}
			if (p.z > cp.z) {
				SDL_RenderDrawPointF(ren, (p.x / (p.z + cp.z)) * 500 + 500 + cp.x, (p.y / (p.z + cp.z)) * 500 + 500 + cp.y);
			}
		}
	}
	SDL_RenderPresent(ren);
}

Point setCRot(float ax, float ay, float az, Point c)
{
	Point o = c;
	o.y = o.y * cos(ax) - o.z * sin(ax);
	o.z = o.y * sin(ax) + o.z * cos(ax);
	o.x = o.x * cos(az) - o.y * sin(az);
	o.y = o.x * sin(az) + o.y * cos(az);
	o.z = o.z * cos(ay) - o.x * sin(ay);
	o.x = o.z * sin(ay) + o.x * cos(ay);
	return o;
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

	// SDL_ShowCursor(SDL_DISABLE);

	int obj = 0;

	float ax = 0;
	float ay = 0;
	float az = 0;
	Point Cp;
	Point Cr;
	Cp.z = -2000;
	Cr.x = 0;
	Cr.y = 0;
	Cr.z = 0;
	Draw(ren, Cp, obj, Cr);

	const float MOV = 20;
	const float SEN = 1;

	char finished = 0;
	// the main event loop
	while (!finished)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_MOUSEMOTION:
				ay += event.motion.xrel * SEN;
				ax += event.motion.yrel * SEN;
				Cr = setCRot(ax, ay, az, Cr);
				// printf("%d\n", event.motion.xrel);
				// printf("%d\n", event.motion.yrel);
				// printf("(%f, %f, %f)\n", Cp.x, Cp.y, Cp.z);
				// printf("(%f, %f, %f)\n", Cr.x, Cr.y, Cr.z);
				Draw(ren, Cp, obj, Cr);
				break;
			case SDL_KEYDOWN:

				switch (event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					finished = 1;
					break;
				case SDLK_w:
					Cp.z += MOV;
					break;
				case SDLK_s:
					Cp.z -= MOV;
					break;
				case SDLK_SPACE:
					Cp.y += MOV;
					break;
				case SDLK_LCTRL:
				case SDLK_RCTRL:
					Cp.y -= MOV;
					break;
				case SDLK_a:
					Cp.x += MOV;
					break;
				case SDLK_d:
					Cp.x -= MOV;
					break;
				case SDLK_0:
					obj = 0;
					break;
				case SDLK_1:
					obj = 1;
					break;
				case SDLK_2:
					obj = 2;
					break;
				}
				Draw(ren, Cp, obj, Cr);
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
