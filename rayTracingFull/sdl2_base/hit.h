#pragma once

#include "point.h"
#include "sphere.h"

class sphere;
class point;

class hit {
    public:
        bool hitSuccess;
        sphere *obj;
        float dist;
        point P;

        hit();
        hit(bool hit, sphere *obj, float dist, point P);
};