#pragma once

#include "vec.h"

class vec;
class color {
public:
    float r;
    float g;
    float b;

    color(float r, float g, float b);
    color();
    vec toVec();
    void toCol(vec i);
    void normaliseF();
    void normalise255();
};