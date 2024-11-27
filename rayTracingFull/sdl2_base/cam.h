#pragma once

#include <SDL2/SDL.h>
#include "point.h"
#include "vec.h"
#include "light.h"
#include "sphere.h"

class light;
class sphere;
class point;
class vec;

class cam {
    public:
        point pos;
        vec dir;
        float fov;
        bool orthographic;

        cam(point pos, vec dir, float fov, bool orthographic);
        cam();

        void draw(SDL_Renderer *ren, light l, sphere *objs, int objCount, int maxWidth, int maxHeight, float sampleAmount, int recursionMax, bool progressive);
        void sample(SDL_Renderer *ren, cam c, light l, sphere *objs, int objCount, int x, int y, float sampleAmount, int maxWidth, int maxHeight, int recursionMax);
};