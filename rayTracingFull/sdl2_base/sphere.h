#pragma once

#include "point.h"
#include "mat.h"
#include "vec.h"
#include "hit.h"

class hit;
class mat;
class vec;
class point;

class sphere {
    public:
        point *pos;
        int *t;
        int keyframeCount;
        float r;
        mat *m;
        int id;

        sphere(point pos, float r, mat* m, int id);
        sphere();

        hit traceObj(point p, vec i, int t);
        vec getNormal(point P, int t);
        void print();
        point getPos(int t);

        void addKeyframe(int t, point p);
};