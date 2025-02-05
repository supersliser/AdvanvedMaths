#include "sphere.h"

sphere::sphere(point pos, float r, mat *m, int id)
{
	this->pos = (point *)malloc(sizeof(point));
	*(this->pos) = pos;
	this->t = (int *)malloc(sizeof(int));
	this->t[0] = 0;
	this->keyframeCount = 1;
	this->r = r;
	this->m = m;
	this->id = id;
}

sphere::sphere()
{
	this->pos = (point *)malloc(sizeof(point));
	this->pos->x = 0;
	this->pos->y = 0;
	this->pos->z = 0;
	this->t = (int *)malloc(sizeof(int));
	this->t[0] = 0;
	this->keyframeCount = 1;
	this->r = 0;
	this->m = new mat();
	this->id = 0;
}

hit sphere::traceObj(point p, vec i, int t)
{
	hit out;
	point O = p;
	// printf("(%f, %f, %f)\n", O.x, O.y, O.z);
	vec D = i;
	point L;
	L.x = this->getPos(t).x - O.x;
	L.y = this->getPos(t).y - O.y;
	L.z = this->getPos(t).z - O.z;
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
		out.hitSuccess = 0;
	}
	else
	{
		out.hitSuccess = 1;
		out.obj = this;
		out.dist = t0;
		out.P = P;
		// printf("Hit at (%f, %f, %f)\n", out->P.x, out->P.y, out->P.z);
	}
	return out;
}

vec sphere::getNormal(point P, int t)
{
	vec N;
	N.x = -(P.x - getPos(t).x);
	N.y = -(P.y - getPos(t).y);
	N.z = -(P.z - getPos(t).z);
	N = N.Normalise();
	return N;
}

point sphere::getPos(int t)
{
	//this function should return the keyframe if it exists, otherwise it should return an interpolation between the two keyframes
	if (keyframeCount == 1)
	{
		return pos[0];
	}
	else
	{
		int i = 0;
		while (this->t[i] < t)
		{
			i++;
		}
		if (this->t[i] == t)
		{
			return pos[i];
		}
		else
		{
			float x = t - this->t[i - 1] / this->t[i] - this->t[i - 1];
			point out;
			out.x = pos[i - 1].x + x * (pos[i].x - pos[i - 1].x);
			out.y = pos[i - 1].y + x * (pos[i].y - pos[i - 1].y);
			out.z = pos[i - 1].z + x * (pos[i].z - pos[i - 1].z);
			return out;
		}
	}
}

void sphere::addKeyframe(int t, point p) {
	for (int i = 0; i < keyframeCount; i++) {
		if (this->t[i] == t) {
			this->pos[i] = p;
			// printf("(%f, %f, %f) keyframe updated\n", p.x, p.y, p.z);
			return;
		}
	}
	point * temp = (point *)calloc((keyframeCount + 1), sizeof(point));
	int * tempT = (int *)calloc((keyframeCount + 1), sizeof(int));
	for (int i = 0; i < keyframeCount; i++) {
		temp[i] = this->pos[i];
		tempT[i] = this->t[i];
	}
	temp[keyframeCount] = p;
	tempT[keyframeCount] = t;
	free(this->pos);
	free(this->t);
	this->pos = temp;
	this->t = tempT;
	keyframeCount++;
}