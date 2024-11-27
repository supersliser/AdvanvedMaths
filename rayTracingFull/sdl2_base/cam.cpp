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

void cam::draw(SDL_Renderer *ren, light l, sphere *objs, int objCount, int maxWidth, int maxHeight, float sampleAmount)
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
                tempCamera.dir.x = 0;
                tempCamera.dir.y = 0;
                tempCamera.dir.z = 1;
            }
            else
            {
                tempCamera.dir.x = (x + tempCamera.dir.x) * (tempCamera.fov / 90);
                tempCamera.dir.y = (y + tempCamera.dir.y) * (tempCamera.fov / 90);
                tempCamera.dir.z = (tempCamera.dir.z) * (tempCamera.fov / 90);
                tempCamera.dir.Normalise();
            }
            tempCamera.sample(ren, tempCamera, l, objs, objCount, x, y, sampleAmount, maxWidth, maxHeight);
            // printf("Sampled (%d, %d)\n", x, y);
            fflush(stdout);
        }
    }
    SDL_RenderPresent(ren);
    printf("rendered\n");
    fflush(stdout);
}

void cam::sample(SDL_Renderer *ren, cam c, light l, sphere *objs, int objCount, int x, int y, float sampleAmount, int maxWidth, int maxHeight)
{
    color output[(int)sampleAmount - 1][(int)sampleAmount - 1];
    for (float u = 0.0; u < 1; u += 1 / (sampleAmount))
    {
        for (float v = 0.0; v < 1; v += 1 / sampleAmount)
        {
            cam tempCam = c;
            tempCam.pos.x += u;
            tempCam.pos.y += v;
            hit *besthit = new hit();
            besthit->P.z = 10000000000;
            for (int i = 0; i < objCount; i++)
            {
                hit h = objs[i].traceObj(tempCam.pos, tempCam.dir);
                if (h.hitSuccess && h.dist < besthit->dist)
                {
                    printf("Hit\n");
                    *besthit = h;
                }
            }
            if (besthit->hitSuccess)
            {
                printf("Hit success at (%f, %f)\n", besthit->P.x, besthit->P.y);
                fflush(stdout);
                output[(int)(u * sampleAmount)][(int)(v * sampleAmount)] = besthit->obj->m->shade(tempCam, l, *besthit, objs, objCount, 0, 2);
            }
            else
            {
                output[(int)(u * sampleAmount)][(int)(v * sampleAmount)] = color(0, 0, 0);
            }
            // printf("Sample (%d, %d) color (%f, %f, %f)\n", (int)(u * sampleAmount), (int)(v * sampleAmount), output[(int)(u * sampleAmount)][(int)(v * sampleAmount)].r, output[(int)(u * sampleAmount)][(int)(v * sampleAmount)].g, output[(int)(u * sampleAmount)][(int)(v * sampleAmount)].b);
        }
    }
    color *pixel = new color();
    for (int i = 0; i < sampleAmount; i++)
    {
        for (int j = 0; j < sampleAmount; j++)
        {
            pixel->r += output[i][j].r;
            pixel->g += output[i][j].g;
            pixel->b += output[i][j].b;
        }
    }
    // printf("Pixel (%d, %d) color (%f, %f, %f)\n", x, y, pixel->r, pixel->g, pixel->b);
    pixel->r = pixel->r / (sampleAmount * sampleAmount);
    pixel->g = pixel->g / (sampleAmount * sampleAmount);
    pixel->b = pixel->b / (sampleAmount * sampleAmount);
    // printf("Pixel (%d, %d) color (%f, %f, %f)\n", x, y, pixel->r, pixel->g, pixel->b);
    pixel->normalise255();
    // printf("Drawing Colour (%d, %d, %d)\n", pixel->r, pixel->g, pixel->b);
    fflush(stdout);
    SDL_SetRenderDrawColor(ren, pixel->r, pixel->g, pixel->b, 255);
    SDL_RenderDrawPoint(ren, x + maxWidth / 2, y + maxHeight / 2);
}