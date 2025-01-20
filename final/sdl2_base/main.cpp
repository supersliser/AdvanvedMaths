#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <complex.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

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

float _Complex *ft(SDL_Renderer *ren, int y, int width)
{
	float data[width];
	float _Complex result[width];

	for (int x = 0; x < width; x++)
	{
		color c = readPixel(ren, x, y);
		float temp = c.r;
		temp /= 255.0;
		data[x] = temp;
	}

	for (int x = 0; x < width; x++)
	{
		result[x] = 0.0 + 0.0 * I;
		for (int xx = 0; xx < width; xx++)
		{
			float angle = 2.0 * M_PI * xx / (float)width;
			result[x] += data[xx] * cexp(I * angle * x);
		}
		result[x] /= width;
	}

	float _Complex *result2 = (float _Complex *)malloc(sizeof(float _Complex) * width);

	for (int x = 0; x < width; x++)
	{
		result2[x] = result[x];
	}

	return result2;
}

float _Complex *Ift(float _Complex * input, int y, int width)
{
	float _Complex data[width];
	float _Complex result[width];

	for (int x = 0; x < width; x++)
	{
		data[x] = input[x];
	}

	for (int x = 0; x < width; x++)
	{
		result[x] = 0.0 + 0.0 * I;
		for (int xx = 0; xx < width; xx++)
		{
			float angle = 2.0 * M_PI * xx / (float)width;
			result[x] += data[xx] * cexp(-I * angle * x);
		}
	}

	float _Complex *result2 = (float _Complex *)malloc(sizeof(float _Complex) * width);

	for (int x = 0; x < width; x++)
	{
		result2[x] = result[x];
	}

	return result2;
}

int main(int argc, char *argv[])
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		printf("error initializing SDL: %s\n", SDL_GetError());
		return 1;
	}

	const int SCREEN_WIDTH = 500;
	const int SCREEN_HEIGHT = 500;

	SDL_Window *win = SDL_CreateWindow("GAME",
									   SDL_WINDOWPOS_CENTERED,
									   SDL_WINDOWPOS_CENTERED,
									   SCREEN_WIDTH, SCREEN_HEIGHT, 0);

	SDL_Renderer *ren = SDL_CreateRenderer(win, -1, 0);
	SDL_RenderClear(ren);
	SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);

	SDL_Surface *image = IMG_Load("start_image3.bmp");
	SDL_Texture *texture = SDL_CreateTextureFromSurface(ren, image);
	SDL_RenderCopy(ren, texture, NULL, NULL);
	SDL_RenderPresent(ren);

	Uint8 pixels[SCREEN_WIDTH * SCREEN_HEIGHT];

	for (int y = 0; y < SCREEN_HEIGHT; y++)
	{
	    for (int x = 0; x < SCREEN_WIDTH; x++)
	    {
	        color c = readPixel(ren, x, y);
	        pixels[SCREEN_WIDTH * y + x] = (c.r + c.g + c.b) / 3;
	    }
	}

	for (int x = 0; x < SCREEN_WIDTH; x++)
	{
		for (int y = 0; y < SCREEN_HEIGHT; y++)
		{
			SDL_SetRenderDrawColor(ren, pixels[SCREEN_WIDTH * y + x], pixels[SCREEN_WIDTH * y + x], pixels[SCREEN_WIDTH * y + x], 255);
			SDL_RenderDrawPoint(ren, x, y);
		}
	}

	SDL_RenderPresent(ren);
	saveImage(ren, "./grayscale_image.bmp", SCREEN_WIDTH, SCREEN_HEIGHT);

	float _Complex *result[SCREEN_HEIGHT];
	for (int y = 0; y < SCREEN_HEIGHT; y++)
	{
		result[y] = ft(ren, y, SCREEN_WIDTH);
	}
	for (int y = 0; y < SCREEN_HEIGHT; y++){
		for (int x = 0; x < SCREEN_WIDTH; x++)
		{
			float temp = cabs(result[y][x]);
			if (temp > 1)
			{
				temp = 1;
			}
			if (temp < 0)
			{
				temp = 0;
			}
			SDL_SetRenderDrawColor(ren, temp * 255, temp * 255, temp * 255, 255);
			SDL_RenderDrawPoint(ren, x, y);
		}
	}
	SDL_RenderPresent(ren);
	saveImage(ren, "./fourier_image.bmp", SCREEN_WIDTH, SCREEN_HEIGHT);

	float _Complex *Ift_result[SCREEN_HEIGHT];
	for (int y = 0; y < SCREEN_HEIGHT; y++)
	{
		Ift_result[y] = Ift(result[y], y, SCREEN_WIDTH);
	}
	for (int y = 0; y < SCREEN_HEIGHT; y++)
	{
		for (int x = 0; x < SCREEN_WIDTH; x++)
		{
			float temp = cabs(Ift_result[y][x]);
			if (temp > 1)
			{
				temp = 1;
			}
			if (temp < 0)
			{
				temp = 0;
			}
			SDL_SetRenderDrawColor(ren, temp * 255, temp * 255, temp * 255, 255);
			SDL_RenderDrawPoint(ren, x, y);
		}
	}
	SDL_RenderPresent(ren);
	saveImage(ren, "./inverse_fourier_image.bmp", SCREEN_WIDTH, SCREEN_HEIGHT);

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
