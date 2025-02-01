#pragma once

#include <SDL2/SDL.h>
#include "point.h"
#include "vec.h"
#include "light.h"
#include "geo.h"
#include "hit.h"

class light;
class point;
class vec;
class geo;
class hit;

enum SampleType {
    LINEAR,
    RANDOM,
    RANDOM_SHRINKING_PIXELS,
    IMPORTANCE,
    IMPORTANCE_IMPROVED,
    IMPORTANCE_JITTERED,
    RANDOM_SHRINKING_PIXELS_MORE_RAYS
};

class cam {
    public:
        point pos;
        vec dir;
        float fov;
        bool orthographic;

        cam(point pos, vec dir, float fov, bool orthographic);
        cam();
        ~cam(); // Add destructor

        void draw(SDL_Renderer *ren, light* l, int lCount, geo *objs, int objCount, int maxWidth, int maxHeight, float sampleAmount, int recursionMax, bool progressive, SampleType sampleType, int passCount, int pixelSize, int importanceStart, int generationEnd, int ImportanceVarianceSize);
        color LinearSample(SDL_Renderer *ren, cam c, light* l, int lCount, geo *objs, int objCount, int x, int y, float sampleAmount, int maxWidth, int maxHeight, int recursionMax, int pixelSize, bool draw);
        color RandomSample(SDL_Renderer *ren, cam c, light* l, int lCount, geo *objs, int objCount, int x, int y, float sampleAmount, int maxWidth, int maxHeight, int recursionMax, int pixelSize, bool drawHere);

        void PathTracePixel(SDL_Renderer *ren, cam c, light* l, int lCount, geo *objs, int objCount, int x, int y, float sampleAmount, int maxWidth, int maxHeight, int recursionMax, int pixelSize);
        color PathTrace(SDL_Renderer *ren, cam c, light* l, int lCount, geo *objs, int objCount, int maxWidth, int maxHeight, int recursionMax, int recursionCount);

        void RandomShrinkingPixels(SDL_Renderer *ren, cam c, light* ls, int lCount, geo *objs, int objCount, int maxWidth, int maxHeight, int recursionMax, int pixelSize, int passCount, int importanceStart, int generationEnd, int importanceVarianceSize, bool progressive, int sampleAmount, int level);

        bool isImportant(color**pixels, int x, int y, int maxWidth, int maxHeight, int importanceVarianceSize, int level);

        void printCam();
};