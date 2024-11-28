#include <stdio.h>
#include <math.h>
#include <time.h>

float f(float x)
{
	return sin(x) / x;
}

float pdf(float x)
{
	float v = 1 - (x / M_PI);
	return v * (2 / M_PI);
}

float getPDFMax(int aCount)
{
	float max = 0;
	for (float i = 0; i < M_PI; i+= 0.01f)
	{
		float v = pdf(i);
		if (v > max)
			max = v;
	}
	return max;
}

int main(int argc, char *argv[])
{
	srandom(time(NULL));

	float rt = 0;
	const int aCount = 100000;

	for (int i = 1; i < aCount; i++)
	{
		float x = ((float)rand() / (float)RAND_MAX) * M_PI;
		// printf("Random number: %f\n", randomNum);
		float px = pdf(x);
		float rn2 = MAXFLOAT;
		while (rn2 > px)
		{
			rn2 = ((float)rand() / (float)RAND_MAX) * getPDFMax(aCount);
		}
		rt += f(x) / px;

		printf("y = %f\n", rt / i);
	}

	float out = rt / (aCount);
	printf("final y = %f\n", out);

	return 0;
}
