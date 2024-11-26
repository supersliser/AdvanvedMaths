#include "cam.h"

cam::cam()
{
    pos.x = 0;
    pos.y = 0;
    pos.z = 0;
    dir.x = 0;
    dir.y = 0;
    dir.z = 1;
    fov = 20;
    nearClip = 1;
    farClip = 1000000;
    orthographic = 0;
}

cam::cam(point pos, vec dir, float fov, float nearClip, float farClip, bool orthographic)
{
    this->pos = pos;
    this->dir = dir;
    this->fov = fov;
    this->nearClip = nearClip;
    this->farClip = farClip;
    this->orthographic = orthographic;
}

void cam::draw(SDL_Renderer *ren, light l, sphere *objs, int objCount, int maxWidth, int maxHeight) {
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
    SDL_RenderClear(ren);
    for (int x = -500; x < 500; x++) {
        for (int y = -500; y < 500; y++) {
            if (this->orthographic) {
                this->pos.
            }
        }
    }
}