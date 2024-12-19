#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include <complex.h>

float f(float x)
{
	return sin(x);
}
float square_wave(float x, int num_terms) {
    double sum = 0.0;
    for (int n = 1; n <= num_terms; n++) {
        sum += sin((2*n - 1) * x) / (2*n - 1);
    }
    return (4.0 / M_PI) * sum;
	}

float _Complex* fourier(float _Complex* x, float _Complex* X, int N) {
	    for (int k = 0; k < N; k++) {
        X[k] = 0;
        for (int n = 0; n < N; n++) {
            double angle = -2 * M_PI * n / N;
            X[k] += x[n] * cexp(I * angle * k);
        }
    }
	return X;
}

float _Complex* fourier_inverse(float _Complex* X, float _Complex* x, int N) {
	    for (int n = 0; n < N; n++) {
		x[n] = 0;
		for (int k = 0; k < N; k++) {
			double angle = 2 * M_PI * k / N;
			x[n] += X[k] * cexp(I * angle * n);
		}
		x[n] *= 1.0 / N;
	}
	return x ;
}
int main(int argc, char *argv[])
{
	// returns zero on success else non-zero
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		printf("error initializing SDL: %s\n", SDL_GetError());
	}
	SDL_Window *win = SDL_CreateWindow("differentiation and reintegration", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1000, 1000, 0);
	SDL_Renderer *ren = SDL_CreateRenderer(win, -1, 0);
	SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
	SDL_RenderClear(ren);

	float _Complex sw[1000];

	// task 1 - red line
	SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
	for (float x = 0; x < 1000; x += 1)
	{
		sw[(int)x] = square_wave(x * 0.01, 1000) * 100 + 0 * I;
		SDL_RenderDrawPoint(ren, x, creal(sw[(int)x]) + 500);
	}

	// SDL_RenderPresent(ren);

	float _Complex fo[1000];
	fourier(sw,fo, 1000);


	// task 2 - blue line
	SDL_SetRenderDrawColor(ren, 0, 0, 255, 255);
	for (int x = 0; x < 1000; x += 1)
	{
		SDL_RenderDrawPoint(ren, (float)x, cabs(fo[x]) + 500);
	}


	// task 3 - green line
	float _Complex fi[1000];
	fourier_inverse(fo,fi, 1000);
	SDL_SetRenderDrawColor(ren, 0, 255, 0, 255);
	for (int x = 0; x < 1000; x += 1)
	{
		SDL_RenderDrawPoint(ren, (float)x, creal(fi[x]) + 500);
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
	IMG_SaveJPG(sectionSurface, "./DifferentiationAndIntegration.jpg", 100);

	return 0;
}
