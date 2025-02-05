#pragma once

#include "point.h"
#include "color.h"
#include "sphere.h"
#include "hit.h"
#include "vec.h"

class sphere;
class hit;
class point;
class color;
class vec;

class light {
public:
    point pos;
    color col;
    float brightness;

    light(point pos, color col, float brightness);
    light();

    bool calculateShadows(hit h, sphere *sphereObjs, int sphereCount, int t);
};