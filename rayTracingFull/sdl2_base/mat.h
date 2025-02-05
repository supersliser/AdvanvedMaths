#pragma once

#include "color.h"
#include "hit.h"
#include "sphere.h"
#include "cam.h"
#include "light.h"

class cam;
class light;
class sphere;
class hit;
class color;
class point;
class vec;

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

    color shade(cam c, light *ls, int lCount, hit h, sphere *objs, int objCount, int recursionCount, int recursionMax, int t);

    color reflect(sphere current, point p, vec normal, vec i, sphere *objs, int objCount, light* ls, int lCount, int recursionCount, int recursionMax, int t);

    void logMat();
};