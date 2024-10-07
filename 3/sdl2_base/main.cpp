#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

void circle(SDL_Renderer *ren, float u, float v, float r) {
	SDL_RenderDrawPoint(ren, r * cos(2 * M_PI * u) * 100 + 500, r * sin(2 * M_PI * u) * 100 + 500);
}

void disk(SDL_Renderer *ren, float u, float v, float r) {
	SDL_RenderDrawPoint(ren, r * v * cos(2 * M_PI * u) * 100 + 500, r * v * sin(2 * M_PI * u) * 100 + 500);
}

void cylinder(SDL_Renderer *ren, float u, float v, float r, float h, float d) {
	float x = r * cos(2 * M_PI * u);
	float y = r * sin(2 * M_PI * u);
	float z = v * h;
	SDL_RenderDrawPoint(ren, (x * d) / (z + d) * 100 + 500, (y * d) / (z + d) * 100 + 500);
}
int main(int argc, char *argv[])
{
	// returns zero on success else non-zero
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("error initializing SDL: %s\n", SDL_GetError());
	}
	SDL_Window* win = SDL_CreateWindow("GAME",
									SDL_WINDOWPOS_CENTERED,
									SDL_WINDOWPOS_CENTERED,
									1000, 1000, 0);


	SDL_Renderer* ren = SDL_CreateRenderer(win, -1, 0);
	SDL_RenderClear(ren);
	SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);

	float r = 1;

	for (float u = 0; u < 1; u+=0.001)
	{
		for (float v = 0; v < 1; v+=0.001)
		{
			SDL_SetRenderDrawColor(ren, 255 * ((u + v) / 2), 255 * ((u + v) / 2), 255 * ((u + v) / 2), 255);
			disk(ren, u, v, r);
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


