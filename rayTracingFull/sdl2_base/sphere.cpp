#include "sphere.h"

sphere::sphere(point pos, float r, color col, int id)
{
    this->pos = pos;
    this->r = r;
    this->col = col;
    this->id = id;
}

sphere::sphere()
{
    this->pos.x = 0;
    this->pos.y = 0;
    this->pos.z = 0;
    this->r = 0;
    this->col.r = 0;
    this->col.g = 0;
    this->col.b = 0;
    this->id = 0;
}

hit sphere::traceObj(point p, vec i)
{
    hit out;
	point O = src;
	vec D = dir;
	point C = obj.pos;
	float r = obj.r;
	point L;
	L.x = C.x - O.x;
	L.y = C.y - O.y;
	L.z = C.z - O.z;
	float tca = dp(L, D);
	float d = sqrt(dp(L, L) - (tca * tca));
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
		out.hit = 0;
	}
	else
	{
		out.hit = 1;
		out.obj = obj;
		out.dist = t0;
		out.P = P;
	}
	return out;
}

vec sphere::getNormal(point P)
{
	vec N;
	N.x = -(P.x - obj.pos.x);
	N.y = -(P.y - obj.pos.y);
	N.z = -(P.z - obj.pos.z);
	N = Normalise(N);
	return N;
}