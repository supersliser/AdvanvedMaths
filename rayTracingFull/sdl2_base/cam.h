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

enum SampleType {
    LINEAR,
    RANDOM,
    LINEAR_PATH_TRACE,
    RANDOM_PATH_TRACE,
    IMPORTANCE,
};

class cam {
    public:
        point pos;
        vec dir;
        float fov;
        bool orthographic;

        cam(point pos, vec dir, float fov, bool orthographic);
        cam();

        void draw(SDL_Renderer *ren, light* l, int lCount, sphere *objs, int objCount, int maxWidth, int maxHeight, float sampleAmount, int recursionMax, bool progressive, SampleType sampleType, int passCount, int pixelSize, int importanceStart, int generationEnd);
        color LinearSample(SDL_Renderer *ren, cam c, light* l, int lCount, sphere *objs, int objCount, int x, int y, float sampleAmount, int maxWidth, int maxHeight, int recursionMax, int pixelSize);
        color RandomSample(SDL_Renderer *ren, cam c, light* l, int lCount, sphere *objs, int objCount, int x, int y, float sampleAmount, int maxWidth, int maxHeight, int recursionMax, int pixelSize, bool drawHere);

        void PathTracePixel(SDL_Renderer *ren, cam c, light* l, int lCount, sphere *objs, int objCount, int x, int y, float sampleAmount, int maxWidth, int maxHeight, int recursionMax, int pixelSize);
        color PathTrace(SDL_Renderer *ren, cam c, light* l, int lCount, sphere *objs, int objCount, int maxWidth, int maxHeight, int recursionMax, int recursionCount);

        static hit TraceObjs(cam c, sphere *objs, int objCount);

        void printCam();
};