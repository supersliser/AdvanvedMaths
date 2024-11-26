#include "point.h"
#include "vec.h"
#include "light.h"
#include "sphere.h"

class cam {
    public:
        point pos;
        vec dir;
        float fov;
        float nearClip;
        float farClip;
        bool orthographic;

        cam(point pos, vec dir, float fov, float nearClip, float farClip, bool orthographic);
        cam();

        void draw(SDL_Renderer *ren, light l, sphere *objs, int objCount);
        void sample(SDL_Renderer *ren, light l, sphere *objs, int objCount, int x, int y, int sampleAmount);
};