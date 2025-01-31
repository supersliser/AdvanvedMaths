#pragma once

#include "vec.h"

class vec;


class point {
    public:
        float x;
        float y;
        float z;

        point(float x, float y, float z);
        point();

        point add(point i);
        vec sub(point i);
        point mult(point i);
        float dp(point i);
        float dp(vec i);
        point cp(point i);

        vec toVec();
};