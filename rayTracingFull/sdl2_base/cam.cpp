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

void cam::draw(SDL_Renderer *ren, light l, sphere *objs, int objCount, int maxWidth, int maxHeight, float sampleAmount, int recursionMax, bool progressive, SampleType sampleType, int passCount)
{
    printf("Starting Draw");
    fflush(stdout);
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
    SDL_RenderClear(ren);
    switch (sampleType)
    {
    case LINEAR:
    case LINEAR_PATH_TRACE:
        for (int x = -maxWidth / 2; x < maxWidth / 2; x++)
        {
            for (int y = -maxHeight / 2; y < maxHeight / 2; y++)
            {
                cam tempCamera = *this;
                if (this->orthographic)
                {
                    tempCamera.pos.x += x;
                    tempCamera.pos.y += y;
                }
                else
                {
                    tempCamera.pos.x += x;
                    tempCamera.pos.y += y;
                    tempCamera.dir.x = (x + tempCamera.dir.x) * (tempCamera.fov / 180);
                    tempCamera.dir.y = (y + tempCamera.dir.y) * (tempCamera.fov / 180);
                    tempCamera.dir.z = (500 * tempCamera.dir.z) * (tempCamera.fov / 180);
                    tempCamera.dir.Normalise();
                }
                switch (sampleType)
                {
                case LINEAR:
                    tempCamera.LinearSample(ren, tempCamera, l, objs, objCount, x, y, sampleAmount, maxWidth, maxHeight, recursionMax);
                    break;
                case LINEAR_PATH_TRACE:
                    tempCamera.PathTracePixel(ren, tempCamera, l, objs, objCount, x, y, sampleAmount, maxWidth, maxHeight, recursionMax);
                    break;
                }
                // printf("Sampled (%d, %d)\n", x, y);
                fflush(stdout);
            }
            if (progressive)
            {
                SDL_RenderPresent(ren);
                printf("Drawn (%d)\n", x);
            }
        }
        break;
    case RANDOM:
    case RANDOM_PATH_TRACE:
        for (int pass = 0; pass < passCount; pass++)
        {
            for (int ray = 0; ray < maxWidth; ray++)
            {
                int x = (rand() / (float)RAND_MAX) * maxWidth - 500;
                int y = (rand() / (float)RAND_MAX) * maxHeight - 500;
                cam tempCamera = *this;
                if (this->orthographic)
                {
                    tempCamera.pos.x += x;
                    tempCamera.pos.y += y;
                }
                else
                {
                    tempCamera.pos.x += x;
                    tempCamera.pos.y += y;
                    tempCamera.dir.x = (x + tempCamera.dir.x) * (tempCamera.fov / 180);
                    tempCamera.dir.y = (y + tempCamera.dir.y) * (tempCamera.fov / 180);
                    tempCamera.dir.z = (500 * tempCamera.dir.z) * (tempCamera.fov / 180);
                    tempCamera.dir.Normalise();
                }
                // printf("Starting Sample\n");
                fflush(stdout);
                switch (sampleType)
                {
                case RANDOM:
                    tempCamera.RandomSample(ren, tempCamera, l, objs, objCount, x, y, sampleAmount, maxWidth, maxHeight, recursionMax);
                    break;
                case RANDOM_PATH_TRACE:
                    tempCamera.PathTracePixel(ren, tempCamera, l, objs, objCount, x, y, sampleAmount, maxWidth, maxHeight, recursionMax);
                    break;
                }
            }
            if (progressive)
            {
                printf("Pass %d/%d\n", pass + 1, passCount);
                fflush(stdout);
                SDL_RenderPresent(ren);
            }
        }
        break;
    }
    SDL_RenderPresent(ren);
    printf("rendered\n");
    fflush(stdout);
}

void cam::LinearSample(SDL_Renderer *ren, cam c, light l, sphere *objs, int objCount, int x, int y, float sampleAmount, int maxWidth, int maxHeight, int recursionMax)
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
            hit h = TraceObjs(tempCam, objs, objCount);
            // printf("Sampling (%f, %f)\n", x, y);
            if (h.hitSuccess)
            {
                // printf("Hit success at (%f, %f)\n", besthit->P.x, besthit->P.y);
                // fflush(stdout);
                output[(int)(u * sampleAmount)][(int)(v * sampleAmount)] = h.obj->m->shade(tempCam, l, h, objs, objCount, 0, recursionMax);
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

void cam::printCam()
{
    printf("Cam: (%f, %f, %f) towards (%f, %f, %f)\n", this->pos.x, this->pos.y, this->pos.z, this->dir.x, this->dir.y, this->dir.z);
    printf("FOV: %f\n", this->fov);
    printf("Orthographic: %d\n", this->orthographic);
    fflush(stdout);
}

void cam::RandomSample(SDL_Renderer *ren, cam c, light l, sphere *objs, int objCount, int x, int y, float sampleAmount, int maxWidth, int maxHeight, int recursionMax)
{
    color output[(int)sampleAmount];
    for (int i = 0; i < sampleAmount; i++)
    {
        cam tempCam = c;
        tempCam.pos.x += (float)rand() / (float)RAND_MAX;
        tempCam.pos.y += (float)rand() / (float)RAND_MAX;
        hit h = TraceObjs(tempCam, objs, objCount);
        if (h.hitSuccess)
        {
            output[i] = h.obj->m->shade(tempCam, l, h, objs, objCount, 0, recursionMax);
        }
        else
        {
            output[i] = color(0, 0, 0);
        }
    }
    color pixel = color();
    for (int i = 0; i < sampleAmount; i++)
    {
        pixel.r += output[i].r;
        pixel.g += output[i].g;
        pixel.b += output[i].b;
    }
    pixel.r = pixel.r / (sampleAmount);
    pixel.g = pixel.g / (sampleAmount);
    pixel.b = pixel.b / (sampleAmount);
    pixel.normaliseF();
    SDL_SetRenderDrawColor(ren, pixel.r * 255, pixel.g * 255, pixel.b * 255, 255);
    SDL_RenderDrawPoint(ren, x + maxWidth / 2, y + maxHeight / 2);
}

void cam::PathTracePixel(SDL_Renderer *ren, cam c, light l, sphere *objs, int objCount, int x, int y, float sampleAmount, int maxWidth, int maxHeight, int recursionMax)
{
    // printf("Starting Path Trace\n");
    // fflush(stdout);
    for (int i = 0; i < 1000; i++)
    {
        cam tempcam = c;
        tempcam.pos.x += (float)rand() / (float)RAND_MAX;
        tempcam.pos.y += (float)rand() / (float)RAND_MAX;
        tempcam.dir = tempcam.dir.randomRay();
        tempcam.dir.Normalise();
        color output = PathTrace(ren, tempcam, l, objs, objCount, maxWidth, maxHeight, recursionMax, 0);
        // printf("Drawing Colour (%f, %f, %f) at (%f, %f)\n", output.r, output.g, output.b, x, y);
        // fflush(stdout);
        SDL_SetRenderDrawColor(ren, output.r * 255, output.g * 255, output.b * 255, 255);
        SDL_RenderDrawPoint(ren, x + maxWidth / 2, y + maxHeight / 2);
    }
}

color cam::PathTrace(SDL_Renderer *ren, cam c, light l, sphere *objs, int objCount, int maxWidth, int maxHeight, int recursionMax, int recursionCount)
{
    // printf("Tracing Path\n");
    // fflush(stdout);
    hit h = TraceObjs(c, objs, objCount);
    c.dir = c.dir.randomRay();
    if (!h.hitSuccess)
    {
        return color(0, 0, 0);
    }
    color output = h.obj->m->shade(c, l, h, objs, objCount, 0, -1);
    // printf("Tracing Shaded\n");
    // fflush(stdout);
    if (recursionCount < recursionMax)
    {
        output.toCol(output.toVec().mult(PathTrace(ren, c, l, objs, objCount, maxWidth, maxHeight, recursionMax, recursionCount + 1).toVec()));
    }
    // printf("Path Traced\n");
    // fflush(stdout);
    return output;
}

hit cam::TraceObjs(cam c, sphere *objs, int objCount)
{
    hit bestHit;
    bestHit.dist = 100000000000000;
    // printf("%d\n", objCount);
    for (int i = 0; i < objCount; i++)
    {
        hit h = objs[i].traceObj(c.pos, c.dir);
        if (h.hitSuccess && h.dist < bestHit.dist)
        {
            bestHit = h;
        }
    }
    return bestHit;
}