#pragma once

#include "color.h"
#include "hit.h"
#include "cam.h"
#include "light.h"

class cam;
class light;
class hit;
class color;
class point;
class vec;
class geo;

class mat
{
public:
    color diffuse;
    color specular;
    color ambient;
    float reflectivity;
    float roughness;

    mat(color diffuse, color specular, color ambient, float reflectivity, float roughness);
    mat();
    ~mat(); // Add destructor

    color shade(cam c, light *ls, int lCount, hit h, geo *objs, int objCount, int recursionCount, int recursionMax);

    color reflect(geo current, point p, vec normal, vec i, geo *objs, int objCount, light* ls, int lCount, int recursionCount, int recursionMax);

    void logMat();
};