#include "point.h"

point::point(float x, float y, float z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}

point::point()
{
    this->x = 0;
    this->y = 0;
    this->z = 0;
}

point point::add(point i)
{
    point o;
    o.x = this->x + i.x;
    o.y = this->y + i.y;
    o.z = this->z + i.z;
    return o;
}

vec point::sub(point i)
{
    vec o;
    o.x = this->x - i.x;
    o.y = this->y - i.y;
    o.z = this->z - i.z;
    return o;
}

point point::mult(point i)
{
    point o;
    o.x = this->x * i.x;
    o.y = this->y * i.y;
    o.z = this->z * i.z;
    return o;
}

float point::dp(point i)
{
    return this->x * i.x + this->y * i.y + this->z * i.z;
}

point point::cp(point i)
{
    point o;
    o.x = this->x * i.x;
    o.y = this->y * i.y;
    o.z = this->z * i.z;
    return o;
}