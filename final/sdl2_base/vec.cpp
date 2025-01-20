#include "vec.h"
#include <math.h>
#include <stdio.h>

vec::vec(float x, float y, float z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}

vec::vec()
{
    this->x = 0;
    this->y = 0;
    this->z = 0;
}

vec vec::add(vec i)
{
    vec o;
    o.x = this->x + i.x;
    o.y = this->y + i.y;
    o.z = this->z + i.z;
    return o;
}

vec vec::sub(vec i)
{
    vec o;
    o.x = this->x - i.x;
    o.y = this->y - i.y;
    o.z = this->z - i.z;
    return o;
}

float vec::dp(vec i)
{
    return this->x * i.x + this->y * i.y + this->z * i.z;
}

vec vec::cp(vec i)
{
    vec o;
    o.x = this->y * i.z - this->z * i.y;
    o.y = this->z * i.x - this->x * i.z;
    o.z = this->x * i.y - this->y * i.x;
    return o;
}

vec vec::mult(vec i)
{
    vec o;
    o.x = this->x * i.x;
    o.y = this->y * i.y;
    o.z = this->z * i.z;
    return o;
}

vec vec::Normalise()
{
    float mag = sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
    vec o;
    o.x = this->x / mag;
    o.y = this->y / mag;
    o.z = this->z / mag;
    return o;
}

vec vec::add(float i)
{
    vec o;
    o.x = this->x + i;
    o.y = this->y + i;
    o.z = this->z + i;
    return o;
}

vec vec::sub(float i)
{
    vec o;
    o.x = this->x - i;
    o.y = this->y - i;
    o.z = this->z - i;
    return o;
}

vec vec::mult(float i)
{
    vec o;
    o.x = this->x * i;
    o.y = this->y * i;
    o.z = this->z * i;
    return o;
}

float vec::length()
{
    return sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
}

vec vec::randomRay()
{
    vec o;
    while (o.z < 1 && o.length() >= 1) {
        o.x = ((float)rand() / (float)RAND_MAX * 2) - 1;
        o.y = ((float)rand() / (float)RAND_MAX * 2) - 1;
        o.z = ((float)rand() / (float)RAND_MAX * 2) - 1;
    }
    o.Normalise();
    // printf("Ray Generated\n");
    // fflush(stdout);
    return o;
}