#pragma once

#include "point.h"

class point;

class vec {
    public:
        float x;
        float y;
        float z;

        vec(float x, float y, float z);
        vec();

        vec add(vec i);
        vec sub(vec i);
        vec mult(vec i);

        vec mult(float i);
        vec add(float i);
        vec sub(float i);

        float dp(vec i);
        vec Normalise();
        vec cp(vec i);
        float mag();

        float length();

        point toPoint();

        vec randomRay();
};