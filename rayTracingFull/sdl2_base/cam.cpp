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
    orthographic = 0;
}

cam::cam(point pos, vec dir, float fov, bool orthographic)
{
    this->pos = pos;
    this->dir = dir;
    this->fov = fov;
    this->orthographic = orthographic;
}

void cam::draw(SDL_Renderer *ren, light l, sphere *objs, int objCount, int maxWidth, int maxHeight, float sampleAmount, int recursionMax, bool progressive)
{
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
    SDL_RenderClear(ren);
    for (int x = -maxWidth / 2; x < maxWidth / 2; x++)
    {
        for (int y = -maxHeight / 2; y < maxHeight / 2; y++)
        {
            cam tempCamera = *this;
            if (this->orthographic)
            {
                tempCamera.pos.x += x;
                tempCamera.pos.y += y;
                tempCamera.pos.z = pos.z;
            }
            else
            {
                tempCamera.dir.x = (x + tempCamera.dir.x) * (tempCamera.fov / 90);
                tempCamera.dir.y = (y + tempCamera.dir.y) * (tempCamera.fov / 90);
                tempCamera.dir.z = (500 * tempCamera.dir.z) * (tempCamera.fov / 90);
                tempCamera.dir.Normalise();
            }
            tempCamera.sample(ren, tempCamera, l, objs, objCount, x, y, sampleAmount, maxWidth, maxHeight, recursionMax);
            // printf("Sampled (%d, %d)\n", x, y);
            fflush(stdout);
            if (progressive)
            {
                SDL_RenderPresent(ren);
            }
        }
    }
    SDL_RenderPresent(ren);
    printf("rendered\n");
    fflush(stdout);
}

void cam::sample(SDL_Renderer *ren, cam c, light l, sphere *objs, int objCount, int x, int y, float sampleAmount, int maxWidth, int maxHeight, int recursionMax)
{
    // printf("Sampling (%d, %d)\n", x, y);
    color output[(int)sampleAmount][(int)sampleAmount];
    for (float u = 0.0; u < 1; u += 1 / (sampleAmount))
    {
        for (float v = 0.0; v < 1; v += 1 / (sampleAmount))
        {
            cam tempCam = c;
            tempCam.pos.x += u;
            tempCam.pos.y += v;
            hit *besthit = new hit();
            besthit->dist = 100000000000000;
            // printf("Temp Cam: (%f, %f, %f) towards (%f, %f, %f)\n", tempCam.pos.x, tempCam.pos.y, tempCam.pos.z, tempCam.dir.x, tempCam.dir.y, tempCam.dir.z);
            for (int i = 0; i < objCount; i++)
            {
                hit h = objs[i].traceObj(tempCam.pos, tempCam.dir);
                if (h.hitSuccess && h.dist < besthit->dist)
                {
                    // printf("Hit\n");
                    *besthit = h;
                }
            }
            // printf("Sampling (%f, %f)\n", x, y);
            if (besthit->hitSuccess)
            {
                // printf("Hit success at (%f, %f)\n", besthit->P.x, besthit->P.y);
                // fflush(stdout);
                output[(int)(u * sampleAmount)][(int)(v * sampleAmount)] = besthit->obj->m->shade(tempCam, l, *besthit, objs, objCount, 0, recursionMax);
            }
            else
            {
                output[(int)(u * sampleAmount)][(int)(v * sampleAmount)] = color(0, 0, 0);
            }
            // printf("Sampling (%f, %f)\n", x, y);
            // printf("Sample (%d, %d) color (%f, %f, %f)\n", (int)(u * sampleAmount), (int)(v * sampleAmount), output[(int)(u * sampleAmount)][(int)(v * sampleAmount)].r, output[(int)(u * sampleAmount)][(int)(v * sampleAmount)].g, output[(int)(u * sampleAmount)][(int)(v * sampleAmount)].b);
        }
    }
    color pixel = color();
    for (int i = 0; i < sampleAmount; i++)
    {
        for (int j = 0; j < sampleAmount; j++)
        {
            pixel.r += output[i][j].r;
            pixel.g += output[i][j].g;
            pixel.b += output[i][j].b;
        }
    }
    // printf("Pixel (%f, %f) color (%f, %f, %f)\n", x, y, pixel.r, pixel.g, pixel.b);
    pixel.r = pixel.r / (sampleAmount * sampleAmount);
    pixel.g = pixel.g / (sampleAmount * sampleAmount);
    pixel.b = pixel.b / (sampleAmount * sampleAmount);
    // printf("Pixel (%f, %f) color (%f, %f, %f)\n", x, y, pixel.r, pixel.g, pixel.b);
    pixel.normaliseF();
    // printf("Drawing Colour (%f, %f, %f) at (%f, %f)\n", pixel.r, pixel.g, pixel.b, x, y);
    fflush(stdout);
    SDL_SetRenderDrawColor(ren, pixel.r * 255, pixel.g * 255, pixel.b * 255, 255);
    SDL_RenderDrawPoint(ren, x + maxWidth / 2, y + maxHeight / 2);
}