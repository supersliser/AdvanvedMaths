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

void cam::draw(SDL_Renderer *ren, light *ls, int lCount, sphere *objs, int objCount, int maxWidth, int maxHeight, float sampleAmount, int recursionMax, bool progressive, SampleType sampleType, int passCount, int pixelSize, int importanceStart)
{
    printf("Starting Draw\n");
    fflush(stdout);
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
    SDL_RenderClear(ren);
    switch (sampleType)
    {
    case LINEAR:
    case LINEAR_PATH_TRACE:
        for (int x = -maxWidth / 2; x < maxWidth / 2; x += pixelSize)
        {
            for (int y = -maxHeight / 2; y < maxHeight / 2; y += pixelSize)
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
                default:
                    tempCamera.LinearSample(ren, tempCamera, ls, lCount, objs, objCount, x, y, sampleAmount, maxWidth, maxHeight, recursionMax, pixelSize);
                    break;
                case LINEAR_PATH_TRACE:
                    tempCamera.PathTracePixel(ren, tempCamera, ls, lCount, objs, objCount, x, y, sampleAmount, maxWidth, maxHeight, recursionMax, pixelSize);
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
                default:
                    // tempCamera.printCam();
                    tempCamera.RandomSample(ren, tempCamera, ls, lCount, objs, objCount, x, y, sampleAmount, maxWidth, maxHeight, recursionMax, pixelSize, 1);
                    break;
                case RANDOM_PATH_TRACE:
                    tempCamera.PathTracePixel(ren, tempCamera, ls, lCount, objs, objCount, x, y, sampleAmount, maxWidth, maxHeight, recursionMax, pixelSize);
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
    case IMPORTANCE:
    {
        color *pixels = (color *)malloc(maxWidth * maxHeight * sizeof(color));
        for (int i = 0; i < maxWidth * maxHeight; i++)
        {
            pixels[i].r = 0;
            pixels[i].g = 0;
            pixels[i].b = 0;
        }
        printf("starting draw\n");
        for (int level = importanceStart; level > 0; level--)
        {
            for (int pass = 0; pass < passCount / level; pass++)
            {
                for (int ray = 0; ray < maxWidth / level; ray++)
                {
                    printf("Starting level %d importance sample\n", level);
                    if (level == importanceStart)
                    {
                        int x = (rand() / (float)RAND_MAX) * maxWidth - 500;
                        int y = (rand() / (float)RAND_MAX) * maxHeight - 500;
                        // printf("Starting sample at (%d, %d)\n", x, y);
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
                        // printf("Sampling (%d, %d)\n", x, y);
                        // printf("%d\n", maxWidth * maxHeight);
                        // printf("%d\n", (((y + maxHeight / 2) + 5) > maxHeight ? maxHeight - 1 : (y + maxHeight / 2) + 5) * maxHeight + (x + maxWidth / 2) + 5);
                        // pixels[(((y + maxHeight / 2) + 5) > maxHeight ? maxHeight - 1 : (y + maxHeight / 2) + 5) * maxHeight + (x + maxWidth / 2) + 5].print();
                        color temp = tempCamera.RandomSample(ren, tempCamera, ls, lCount, objs, objCount, x, y, sampleAmount, maxWidth, maxHeight, recursionMax, level * pixelSize, 0);
                        // printf("Sampled successfully, applying sample to array\n");
                        for (int u = 0; u < level * pixelSize; u++)
                        {
                            for (int v = 0; v < level * pixelSize; v++)
                            {
                                // printf("%d\n", (((y + maxHeight / 2) + v) > maxHeight ? maxHeight - 1 : (y + maxHeight / 2) + v) * maxHeight + (x + maxWidth / 2) + u);
                                pixels[(((y + maxHeight / 2) + v) >= maxHeight ? maxHeight - 1 : (y + maxHeight / 2) + v) * maxHeight + (x + maxWidth / 2) + u] = temp;
                            }
                        }

                        // printf("Sampled (%d, %d)\n", x, y);
                    }
                    else
                    {
                        cam tempCamera = *this;
                        int x = 0;
                        int y = 0;
                        do
                        {
                            x = (rand() / (float)RAND_MAX) * (maxWidth / 2);
                            y = (rand() / (float)RAND_MAX) * (maxHeight / 2);
                            // printf("x: %d, y: %d\n", x, y);
                            if (this->orthographic)
                            {
                                tempCamera.pos.x = x;
                                tempCamera.pos.y = y;
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
                            // printf("Sampling (%d, %d)\n", tempCamera.pos.x + 500, tempCamera.pos.y + 500);
                            // printf("%d\n", (int)floor((tempCamera.pos.x + 500) * maxWidth + (tempCamera.pos.y + 500)));
                            // printf("Pixel color (%f, %f, %f) at (%d, %d)\n", pixels[(int)floor((tempCamera.pos.x + 500) * maxWidth + (tempCamera.pos.y + 500))].r, pixels[(int)floor((tempCamera.pos.x + 500) * maxWidth + (tempCamera.pos.y + 500))].g, pixels[(int)floor((tempCamera.pos.x + 500) * maxWidth + (tempCamera.pos.y + 500))].b, x + 500, y + 500);
                        } while (!pixels[((y + maxHeight / 2) > maxHeight ? maxHeight - 1 : y + maxHeight / 2) * maxHeight + (x + maxWidth / 2)].notBlack());
                        // printf("Found not black\n");
                        color temp = tempCamera.RandomSample(ren, tempCamera, ls, lCount, objs, objCount, tempCamera.pos.x, tempCamera.pos.y, sampleAmount, maxWidth, maxHeight, recursionMax, level * pixelSize, 0);
                        for (int u = 0; u < level * pixelSize; u++)
                        {
                            for (int v = 0; v < level * pixelSize; v++)
                            {
                                pixels[(((y + maxHeight / 2) + v) > maxHeight ? maxHeight - 1 : (y + maxHeight / 2) + v) * maxHeight + (x + maxWidth / 2) + u] = temp;
                            }
                        }
                    }
                }
            }
            for (int x = 0; x < maxWidth / level; x++)
            {
                for (int y = 0; y < maxHeight / level; y++)
                {
                    SDL_SetRenderDrawColor(ren, pixels[y * maxWidth + x].r * 255, pixels[y * maxWidth + x].g * 255, pixels[y * maxWidth + x].b * 255, 255);
                    SDL_RenderDrawPoint(ren, x, y);
                }
            }
            if (progressive)
            {

                SDL_RenderPresent(ren);
            }
        }

        free(pixels);
    }
    break;
    }
    SDL_RenderPresent(ren);
    printf("rendered\n");
    fflush(stdout);
}

color cam::LinearSample(SDL_Renderer *ren, cam c, light *ls, int lCount, sphere *objs, int objCount, int x, int y, float sampleAmount, int maxWidth, int maxHeight, int recursionMax, int pixelSize)
{
    // printf("Sampling (%d, %d)\n", x, y);
    color output[(int)sampleAmount][(int)sampleAmount];
    for (float u = 0.0; u < pixelSize; u += 1 / (sampleAmount)*pixelSize)
    {
        for (float v = 0.0; v < pixelSize; v += 1 / (sampleAmount)*pixelSize)
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
                output[(int)(u * sampleAmount)][(int)(v * sampleAmount)] = h.obj->m->shade(tempCam, ls, lCount, h, objs, objCount, 0, recursionMax);
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
    for (int i = 0; i < pixelSize; i++)
    {
        for (int j = 0; j < pixelSize; j++)
        {
            SDL_RenderDrawPoint(ren, x + i + (maxWidth / 2), y + j + (maxHeight / 2));
        }
    }
    return pixel;
}

void cam::printCam()
{
    printf("Cam: (%f, %f, %f) towards (%f, %f, %f)\n", this->pos.x, this->pos.y, this->pos.z, this->dir.x, this->dir.y, this->dir.z);
    printf("FOV: %f\n", this->fov);
    printf("Orthographic: %d\n", this->orthographic);
    fflush(stdout);
}

color cam::RandomSample(SDL_Renderer *ren, cam c, light *ls, int lCount, sphere *objs, int objCount, int x, int y, float sampleAmount, int maxWidth, int maxHeight, int recursionMax, int pixelSize, bool drawHere)
{
    color output[(int)sampleAmount];
    for (int i = 0; i < sampleAmount; i++)
    {
        cam tempCam = c;
        tempCam.pos.x += (float)rand() / (float)RAND_MAX;
        tempCam.pos.y += (float)rand() / (float)RAND_MAX;
        hit h = TraceObjs(tempCam, objs, objCount);
        // printf("Traced objects\n");
        if (h.hitSuccess)
        {
            output[i] = h.obj->m->shade(tempCam, ls, lCount, h, objs, objCount, 0, recursionMax);
            // printf("shaded object\n");
        }
        else
        {
            output[i] = color(0, 0, 0);
            // printf("no object found\n");
        }
        // printf("Got sample %d\n", i);
    }
    color pixel = color();
    for (int i = 0; i < sampleAmount; i++)
    {
        pixel.r += output[i].r;
        pixel.g += output[i].g;
        pixel.b += output[i].b;
    }
    // pixel.print();
    pixel.r = pixel.r / (sampleAmount);
    pixel.g = pixel.g / (sampleAmount);
    pixel.b = pixel.b / (sampleAmount);
    pixel.normaliseF();
    // printf("got pixel\n");
    // printf("%d\n", drawHere);
    // if (drawHere)
    // {
    SDL_SetRenderDrawColor(ren, pixel.r * 255, pixel.g * 255, pixel.b * 255, 255);
    // printf("Drawing Colour (%f, %f, %f) at (%d, %d)\n", pixel.r, pixel.g, pixel.b, x, y);
    for (int i = 0; i < pixelSize; i++)
    {
        for (int j = 0; j < pixelSize; j++)
        {
            SDL_RenderDrawPoint(ren, x + i + (maxWidth / 2), y + j + (maxHeight / 2));
        }
    }
    // printf("drawn pixel\n");
    // }
    // printf("Returning pixel\n");
    return pixel;
}

void cam::PathTracePixel(SDL_Renderer *ren, cam c, light *ls, int lCount, sphere *objs, int objCount, int x, int y, float sampleAmount, int maxWidth, int maxHeight, int recursionMax, int pixelSize)
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
        color output = PathTrace(ren, tempcam, ls, lCount, objs, objCount, maxWidth, maxHeight, recursionMax, 0);
        // printf("Drawing Colour (%f, %f, %f) at (%f, %f)\n", output.r, output.g, output.b, x, y);
        // fflush(stdout);
        SDL_SetRenderDrawColor(ren, output.r * 255, output.g * 255, output.b * 255, 255);
        for (int i = 0; i < pixelSize; i++)
        {
            for (int j = 0; j < pixelSize; j++)
            {
                SDL_RenderDrawPoint(ren, x + i + (maxWidth / 2), y + j + (maxHeight / 2));
            }
        }
    }
}

color cam::PathTrace(SDL_Renderer *ren, cam c, light *ls, int lCount, sphere *objs, int objCount, int maxWidth, int maxHeight, int recursionMax, int recursionCount)
{
    // printf("Tracing Path\n");
    // fflush(stdout);
    hit h = TraceObjs(c, objs, objCount);
    c.dir = c.dir.randomRay();
    if (!h.hitSuccess)
    {
        return color(0, 0, 0);
    }
    color output = h.obj->m->shade(c, ls, lCount, h, objs, objCount, 0, -1);
    // printf("Tracing Shaded\n");
    // fflush(stdout);
    if (recursionCount < recursionMax)
    {
        output.toCol(output.toVec().mult(PathTrace(ren, c, ls, lCount, objs, objCount, maxWidth, maxHeight, recursionMax, recursionCount + 1).toVec()));
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