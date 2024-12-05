#include <stdio.h>
#include <math.h>
#include <time.h>

double f(float x)
{
	return sin(x) / x;
}

double pdf(double x)
{
	float v = 1 - (x / M_PI);
	return v * (2 / M_PI);
}

double getPDFMax(int aCount)
{
	double max = 0;
	for (double i = 0; i < M_PI; i+= 0.01f)
	{
		double v = pdf(i);
		if (v > max)
			max = v;
	}
	return max;
}

int main(int argc, char *argv[])
{
	srandom(time(NULL));

	double rt = 0;
	const int aCount = 100000;

	for (int i = 1; i < aCount; i++)
	{
		double x = ((double)rand() / (double)RAND_MAX) * M_PI;
		// printf("Random number: %f\n", randomNum);
		double px = pdf(x);
		double rn2 = MAXFLOAT;
		while (rn2 > px)
		{
			rn2 = ((double)rand() / (double)RAND_MAX) * getPDFMax(aCount);
		}
		rt += f(x) / px;

		printf("y = %f\n", rt / i);
	}

	double out = rt / (aCount);
	printf("final y = %f\n", out);

	return 0;
}
