#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
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
	SDL_Rect src;
	src.x = 50;
	src.y = 50;
	src.w = 50;
	src.h = 50;
	SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
	SDL_RenderDrawRect(ren, &src);

	SDL_RenderDrawPoint(ren, 250, 250);
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


