#include "sphere.h"
#include "point.h"

class hit {
    public:
        bool hit;
        sphere obj;
        float dist;
        point P;

        hit();
        hit(bool hit, sphere obj, float dist, point P);
};