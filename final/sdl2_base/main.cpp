#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <complex.h>

void saveImage(SDL_Renderer *ren, char *filename, int width, int height)
{
	SDL_Surface *sectionSurface = SDL_CreateRGBSurface(0, width, height, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
	SDL_RenderReadPixels(ren, NULL, SDL_PIXELFORMAT_RGBA8888, sectionSurface->pixels, sectionSurface->pitch);
	SDL_SaveBMP(sectionSurface, filename);
	SDL_FreeSurface(sectionSurface);
}

typedef struct color
{
	Uint8 r;
	Uint8 g;
	Uint8 b;
	Uint8 a;
} color;

color readPixel(SDL_Renderer *ren, int x, int y)
{
	SDL_Rect rect = {x, y, 1, 1};
	SDL_Surface *sectionSurface = SDL_CreateRGBSurface(0, 1, 1, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
	SDL_RenderReadPixels(ren, &rect, SDL_PIXELFORMAT_RGBA8888, sectionSurface->pixels, sectionSurface->pitch);
	color c = {0, 0, 0, 0};
	Uint32 pixel = ((Uint32 *)sectionSurface->pixels)[0];
	SDL_GetRGBA(pixel, sectionSurface->format, &c.r, &c.g, &c.b, &c.a);
	SDL_FreeSurface(sectionSurface);
	return c;
}

int *ft(SDL_Renderer *ren, int y, int width)
{
	float _Complex data[width];
	float _Complex result[width];

	for (int x = 0; x < width; x++)
	{
		color c = readPixel(ren, x, y);
		data[x] = c.r + 0.0 * I;
	}

	for (int x = 0; x < width; x++)
	{
		result[x] = 0.0 + 0.0 * I;
		for (int xx = 0; xx < width; xx++)
		{
			float angle = 2.0 * M_PI * x * xx / (float)width;
			result[x] += data[xx] * cexp(I * angle);
		}
	}
	int *output = (int *)malloc(sizeof(int) * width);
	for (int i = 0; i < width; i++)
	{
		output[i] = cabs(result[i]);
	}
	return output;
}

int *Ift(SDL_Renderer *ren, int y, int width)
{
	float _Complex data[width];
	float _Complex result[width];

	for (int x = 0; x < width; x++)
	{
		color c = readPixel(ren, x, y);
		data[x] = c.r + 0.0 * I;
	}

	for (int x = 0; x < width; x++)
	{
		result[x] = 0.0 + 0.0 * I;
		for (int xx = 0; xx < width; xx++)
		{
			float angle = 2.0 * M_PI * x * xx / (float)width;
			result[x] += data[xx] * cexp(I * -angle);
		}
				result[x] /= width;
	}

	int *output = (int *)malloc(sizeof(int) * width);
	for (int i = 0; i < width; i++)
	{
		output[i] = cabs(result[i]);
	}
	return output;
}

int main(int argc, char *argv[])
{
	// returns zero on success else non-zero
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		printf("error initializing SDL: %s\n", SDL_GetError());
	}

	const int SCREEN_WIDTH = 200;
	const int SCREEN_HEIGHT = 200;

	SDL_Window *win = SDL_CreateWindow("GAME",
									   SDL_WINDOWPOS_CENTERED,
									   SDL_WINDOWPOS_CENTERED,
									   SCREEN_WIDTH, SCREEN_HEIGHT, 0);

	SDL_Renderer *ren = SDL_CreateRenderer(win, -1, 0);
	SDL_RenderClear(ren);
	SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);

	// load an image
	SDL_Surface *image = IMG_Load("start_image.bmp");

	// create a texture
	SDL_Texture *texture = SDL_CreateTextureFromSurface(ren, image);

	SDL_RenderCopy(ren, texture, NULL, NULL);

	// convert to greyscale
	for (int row = 0; row < SCREEN_HEIGHT; row++)
	{
		for (int col = 0; col < SCREEN_WIDTH; col++)
		{
			color c = readPixel(ren, col, row);
			// Uint8 grey = (Uint8)((c.r + c.g + c.b) / 3);
			SDL_SetRenderDrawColor(ren, c.r, c.r, c.r, 255);
			SDL_RenderDrawPoint(ren, col, row);
		}
	}
    SDL_RenderPresent(ren);
	saveImage(ren, "./greyscale_image.bmp", SCREEN_WIDTH, SCREEN_HEIGHT);

	for (int v = 0; v < SCREEN_HEIGHT; v++)
	{
		printf("%d\n", v);
		int *result = ft(ren, v, SCREEN_WIDTH);
		for (int u = 0; u < SCREEN_WIDTH; u++)
		{
			if (result[u] > 255)
			{
				result[u] = 255;
			}
			if (result[u] < 0)
			{
				result[u] = 0;
			}
			SDL_SetRenderDrawColor(ren, result[u], result[u], result[u], 255);
			SDL_RenderDrawPoint(ren, u, v);
		}
		free(result);
	}

	saveImage(ren, "./fourier_image.bmp", SCREEN_WIDTH, SCREEN_HEIGHT);

	for (int v = 0; v < SCREEN_HEIGHT; v++)
	{
		printf("%d\n", v);
		int *result = Ift(ren, v, SCREEN_WIDTH);
		for (int u = 0; u < SCREEN_WIDTH; u++)
		{
			if (result[u] > 255)
			{
				result[u] = 255;
			}
			if (result[u] < 0)
			{
				result[u] = 0;
			}
			SDL_SetRenderDrawColor(ren, result[u], result[u], result[u], 255);
			SDL_RenderDrawPoint(ren, u, v);
		}
		free(result);
	}

	saveImage(ren, "./inverse_fourier_image.bmp", SCREEN_WIDTH, SCREEN_HEIGHT);

	SDL_RenderPresent(ren);

	while (1)
	{
		SDL_Event e;
		if (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
			{
				break;
			}
		}
	}

	saveImage(ren, "./end_image.bmp", SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_DestroyTexture(texture);
	SDL_FreeSurface(image);
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);

	IMG_Quit();
	SDL_Quit();

	return 0;
}
