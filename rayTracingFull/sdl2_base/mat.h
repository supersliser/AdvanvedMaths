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

    color shade(cam c, light l, hit h, sphere *objs, int objCount, int recursionCount, int recursionMax);

    color reflect(sphere current, point p, vec normal, vec i, sphere *objs, int objCount, light l, int recursionCount, int recursionMax);

    void logMat();
};