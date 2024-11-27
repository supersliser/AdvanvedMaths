#include "sphere.h"

sphere::sphere(point pos, float r, mat* m, int id)
{
    this->pos = pos;
    this->r = r;
    this->m = m;
    this->id = id;
}

sphere::sphere()
{
    this->pos.x = 0;
    this->pos.y = 0;
    this->pos.z = 0;
    this->r = 0;
	this->m = new mat();
    this->id = 0;
}

hit sphere::traceObj(point p, vec i)
{
    hit* out = new hit();
	point O = p;
	vec D = i;
	point C = pos;
	point L;
	L.x = C.x - O.x;
	L.y = C.y - O.y;
	L.z = C.z - O.z;
	float tca = L.dp(D);
	float d = sqrt(L.dp(L) - (tca * tca));
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
	// printf("(%f, %f, %f)\n", P.x, P.y, P.z);
	if (isnan(P.x))
	{
		out->hitSuccess = 0;
	}
	else
	{
		out->hitSuccess = 1;
		out->obj = this;
		out->dist = t0;
		out->P = P;
	}
	return *out;
}

vec sphere::getNormal(point P)
{
	vec N;
	N.x = -(P.x - pos.x);
	N.y = -(P.y - pos.y);
	N.z = -(P.z - pos.z);
	N = N.Normalise();
	return N;
}