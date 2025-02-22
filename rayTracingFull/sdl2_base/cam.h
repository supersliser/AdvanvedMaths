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

        void draw(SDL_Renderer *ren, light* l, int lCount, sphere *objs, int objCount, int maxWidth, int maxHeight, float sampleAmount, int recursionMax, bool progressive, SampleType sampleType, int passCount, int pixelSize, int importanceStart, int generationEnd, int ImportanceVarianceSize, int t);
        color LinearSample(SDL_Renderer *ren, cam c, light* l, int lCount, sphere *objs, int objCount, int x, int y, float sampleAmount, int maxWidth, int maxHeight, int recursionMax, int pixelSize, bool draw, int t);
        color RandomSample(SDL_Renderer *ren, cam c, light* l, int lCount, sphere *objs, int objCount, int x, int y, float sampleAmount, int maxWidth, int maxHeight, int recursionMax, int pixelSize, bool drawHere, int t);

        void RandomShrinkingPixels(SDL_Renderer *ren, cam c, light* ls, int lCount, sphere *objs, int objCount, int maxWidth, int maxHeight, int recursionMax, int pixelSize, int passCount, int importanceStart, int generationEnd, int importanceVarianceSize, bool progressive, int sampleAmount, int level, int t);

        static hit TraceObjs(cam c, sphere *objs, int objCount, int t);

        bool isImportant(color**pixels, int x, int y, int maxWidth, int maxHeight, int importanceVarianceSize, int level);

        void printCam();

        void renderFrames(SDL_Renderer *ren, light* l, int lCount, sphere *objs, int objCount, int maxWidth, int maxHeight, float sampleAmount, int recursionMax, bool progressive, SampleType sampleType, int passCount, int pixelSize, int importanceStart, int generationEnd, int importanceVarianceSize, int start, int end);
};