#include "point.h"
#include "color.h"
#include "sphere.h"
#include "hit.h"

class light {
public:
    point pos;
    color col;
    float brightness;

    light(point pos, color col, float brightness);
    light();

    bool calculateShadows(hit h, sphere *sphereObjs, int sphereCount);
};