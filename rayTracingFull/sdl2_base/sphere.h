#include "point.h"
#include "mat.h"
#include "vec.h"
#include "hit.h"

class sphere {
    public:
        point pos;
        float r;
        mat m;
        int id;

        sphere(point pos, float r, mat m, int id);
        sphere();

        hit traceObj(point p, vec i);
        vec getNormal(point P);
};