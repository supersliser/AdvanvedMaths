#pragma once

#include "point.h"
#include "geo.h"

class point;
class geo;
class face;

class hit {
    public:
        bool hitSuccess;
        geo *obj;
        float dist;
        point P;
        face *f;

        hit();
        hit(bool hit, geo *obj, float dist, point P, face *f);
};