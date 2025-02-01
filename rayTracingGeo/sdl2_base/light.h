#pragma once

#include "point.h"
#include "color.h"
#include "hit.h"
#include "vec.h"
#include "geo.h"

class hit;
class point;
class color;
class vec;
class geo;
class face;

class light {
public:
    point pos;
    color col;
    float brightness;

    light(point pos, color col, float brightness);
    light();
    ~light(); // Add destructor

    bool calculateShadows(hit h, geo *sphereObjs, int sphereCount);
};