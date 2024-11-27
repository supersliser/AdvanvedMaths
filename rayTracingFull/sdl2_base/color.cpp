#include "color.h"
#include <math.h>

color::color(float r, float g, float b)
{
    this->r = r;
    this->g = g;
    this->b = b;
}

color::color()
{
    this->r = 0;
    this->g = 0;
    this->b = 0;
}

vec color::toVec()
{
    vec o;
    o.x = r;
    o.y = g;
    o.z = b;
    return o;
}

void color::toCol(vec i)
{
    r = i.x;
    g = i.y;
    b = i.z;
}

void color::normaliseF()
{
    if (r > 1)
    {
        r = 1;
    }
    if (r < 0 || isnan(r))
    {
        r = 0;
    }
    if (g > 1)
    {
        g = 1;
    }
    if (g < 0 || isnan(g))
    {
        g = 0;
    }
    if (b > 1)
    {
        b = 1;
    }
    if (b < 0 || isnan(b))
    {
        b = 0;
    }
}

void color::normalise255()
{
    if (r > 255)
    {
        r = 255;
    }
    if (r < 0)
    {
        r = 0;
    }
    if (g > 255)
    {
        g = 255;
    }
    if (g < 0)
    {
        g = 0;
    }
    if (b > 255)
    {
        b = 255;
    }
    if (b < 0)
    {
        b = 0;
    }
}