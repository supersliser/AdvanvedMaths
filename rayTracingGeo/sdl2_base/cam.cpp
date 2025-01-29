#include "cam.h"
#include <thread>
#include <vector>

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

void cam::RandomShrinkingPixels(SDL_Renderer *ren, cam c, light *ls, int lCount, geo *objs, int objCount, int maxWidth, int maxHeight, int recursionMax, int pixelSize, int passCount, int importanceStart, int generationEnd, int importanceVarianceSize, bool progressive, int sampleAmount, int level)
{
    int x = (rand() / (float)RAND_MAX) * maxWidth - maxWidth / 2;
    int y = (rand() / (float)RAND_MAX) * maxHeight - maxHeight / 2;
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
    color temp = tempCamera.RandomSample(ren, tempCamera, ls, lCount, objs, objCount, x + (rand() / (float)RAND_MAX) * pixelSize * level, y + (rand() / (float)RAND_MAX) * pixelSize * level, sampleAmount, maxWidth, maxHeight, recursionMax, level * pixelSize, 0);
    for (int u = 0; u < level * pixelSize; u++)
    {
        for (int v = 0; v < level * pixelSize; v++)
        {
            SDL_SetRenderDrawColor(ren, temp.r * 255, temp.g * 255, temp.b * 255, 255);
            SDL_RenderDrawPoint(ren, SDL_clamp(x + u + (maxWidth / 2), 0, maxWidth), SDL_clamp(y + v + (maxHeight / 2), 0, maxHeight));
        }
    }
}

void cam::draw(SDL_Renderer *ren, light *ls, int lCount, geo *objs, int objCount, int maxWidth, int maxHeight, float sampleAmount, int recursionMax, bool progressive, SampleType sampleType, int passCount, int pixelSize, int importanceStart, int generationEnd, int importanceVarianceSize)
{
    printf("Starting Draw\n");
    fflush(stdout);
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
    SDL_RenderClear(ren);
    switch (sampleType)
    {
    case LINEAR:
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
                printf("Beginning sample at (%d, %d)\n", x, y);
                    tempCamera.LinearSample(ren, tempCamera, ls, lCount, objs, objCount, x, y, sampleAmount, maxWidth, maxHeight, recursionMax, pixelSize, 1);
                fflush(stdout);
            }
            if (progressive)
            {
                SDL_RenderPresent(ren);
                printf("Drawn (y=%d)\n", x);
            }
        }
        break;
    case RANDOM:
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
                fflush(stdout);
                switch (sampleType)
                {
                case RANDOM:
                default:
                    tempCamera.RandomSample(ren, tempCamera, ls, lCount, objs, objCount, x, y, sampleAmount, maxWidth, maxHeight, recursionMax, pixelSize, 1);
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
    case RANDOM_SHRINKING_PIXELS:
    {
        for (int level = importanceStart; level > 0; level--)
        {
            std::vector<std::thread> threads;

            printf("Starting level %d importance sample\n", level);
            for (int pass = 0; pass < passCount / level; pass++)
            {
                threads.push_back(std::thread([=]()
                                              { RandomShrinkingPixels(ren, *this, ls, lCount, objs, objCount, maxWidth, maxHeight, recursionMax, pixelSize, passCount, importanceStart, generationEnd, importanceVarianceSize, progressive, sampleAmount, level); }));
                threads.back().join();
            }
            if (progressive)
            {
                SDL_RenderPresent(ren);
            }
        }
        break;
    }
    case RANDOM_SHRINKING_PIXELS_MORE_RAYS:
    {
        for (int level = importanceStart; level > 0; level--)
        {
            printf("Starting level %d importance sample\n", level);
            for (int pass = 0; pass < passCount / level; pass++)
            {
                for (int ray = 0; ray < maxWidth / level; ray++)
                {
                    int x = (rand() / (float)RAND_MAX) * maxWidth - maxWidth / 2;
                    int y = (rand() / (float)RAND_MAX) * maxHeight - maxHeight / 2;
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
                    color temp = tempCamera.RandomSample(ren, tempCamera, ls, lCount, objs, objCount, x, y, sampleAmount, maxWidth, maxHeight, recursionMax, level * pixelSize, 0);
                    for (int u = 0; u < level * pixelSize; u++)
                    {
                        for (int v = 0; v < level * pixelSize; v++)
                        {
                            SDL_SetRenderDrawColor(ren, temp.r * 255, temp.g * 255, temp.b * 255, 255);
                            SDL_RenderDrawPoint(ren, SDL_clamp(x + u + (maxWidth / 2), 0, maxWidth), SDL_clamp(y + v + (maxHeight / 2), 0, maxHeight));
                        }
                    }
                }
            }
            if (progressive)
            {
                SDL_RenderPresent(ren);
            }
        }
        break;
    }
    case IMPORTANCE:
    {
        color *pixels = (color *)malloc(maxWidth * maxHeight * sizeof(color));
        for (int i = 0; i < maxWidth * maxHeight; i++)
        {
            pixels[i].r = 0;
            pixels[i].g = 0;
            pixels[i].b = 0;
        }
        for (int level = importanceStart; level > 0; level--)
        {
            printf("Starting level %d importance sample\n", level);
            for (int pass = 0; pass < passCount / level; pass++)
            {
                for (int ray = 0; ray < maxWidth / level; ray++)
                {
                    if (level >= generationEnd)
                    {
                        int x = (rand() / (float)RAND_MAX) * maxWidth - maxWidth / 2;
                        int y = (rand() / (float)RAND_MAX) * maxHeight - maxHeight / 2;
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
                        color temp = tempCamera.RandomSample(ren, tempCamera, ls, lCount, objs, objCount, x, y, sampleAmount, maxWidth, maxHeight, recursionMax, level * pixelSize, 0);
                        for (int u = 0; u < level * pixelSize; u++)
                        {
                            for (int v = 0; v < level * pixelSize; v++)
                            {
                                pixels[(((y + maxHeight / 2) + v) >= maxHeight ? maxHeight - 1 : (y + maxHeight / 2) + v) * maxHeight + (((x + maxWidth / 2) + u) >= maxWidth ? maxWidth - 1 : (x + maxWidth / 2) + u)] = temp;
                            }
                        }
                    }
                    else
                    {
                        cam tempCamera;
                        int x = 0;
                        int y = 0;
                        do
                        {
                            tempCamera = *this;
                            x = (rand() / (float)RAND_MAX) * maxWidth - (maxWidth / 2);
                            y = (rand() / (float)RAND_MAX) * maxHeight - (maxHeight / 2);
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
                        } while (!pixels[((y + maxHeight / 2) >= maxHeight ? maxHeight - 1 : y + maxHeight / 2) * maxHeight + ((x + maxWidth / 2) >= maxWidth ? maxWidth - 1 : x + maxWidth / 2)].notBlack());
                        color temp = tempCamera.RandomSample(ren, tempCamera, ls, lCount, objs, objCount, tempCamera.pos.x, tempCamera.pos.y, sampleAmount, maxWidth, maxHeight, recursionMax, level * pixelSize, 0);
                        for (int u = 0; u < level * pixelSize; u++)
                        {
                            for (int v = 0; v < level * pixelSize; v++)
                            {
                                pixels[(((y + maxHeight / 2) + v) >= maxHeight ? maxHeight - 1 : (y + maxHeight / 2) + v) * maxHeight + (((x + maxWidth / 2) + u) >= maxWidth ? maxWidth - 1 : (x + maxWidth / 2) + u)] = temp;
                            }
                        }
                    }
                }
            }
            for (int x = 0; x < maxWidth; x++)
            {
                for (int y = 0; y < maxHeight; y++)
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
    case IMPORTANCE_IMPROVED:
    {
        color *pixels = (color *)malloc(maxWidth * maxHeight * sizeof(color));
        for (int i = 0; i < maxWidth * maxHeight; i++)
        {
            pixels[i].r = 0;
            pixels[i].g = 0;
            pixels[i].b = 0;
        }
        for (int level = importanceStart; level > 0; level--)
        {
            printf("Starting level %d importance sample\n", level);
            for (int pass = 0; pass < passCount / level; pass++)
            {
                for (int ray = 0; ray < maxWidth / level; ray++)
                {
                    if (level >= generationEnd)
                    {
                        int x = (rand() / (float)RAND_MAX) * maxWidth - maxWidth / 2;
                        int y = (rand() / (float)RAND_MAX) * maxHeight - maxHeight / 2;
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
                        color temp = tempCamera.RandomSample(ren, tempCamera, ls, lCount, objs, objCount, x, y, sampleAmount, maxWidth, maxHeight, recursionMax, level * pixelSize, 0);
                        for (int u = 0; u < level * pixelSize; u++)
                        {
                            for (int v = 0; v < level * pixelSize; v++)
                            {
                                pixels[(((y + maxHeight / 2) + v) >= maxHeight ? maxHeight - 1 : (y + maxHeight / 2) + v) * maxHeight + (((x + maxWidth / 2) + u) >= maxWidth ? maxWidth - 1 : (x + maxWidth / 2) + u)] = temp;
                            }
                        }
                    }
                    else
                    {
                        cam tempCamera;
                        int x = 0;
                        int y = 0;
                        do
                        {
                            tempCamera = *this;
                            x = (rand() / (float)RAND_MAX) * maxWidth - (maxWidth / 2);
                            y = (rand() / (float)RAND_MAX) * maxHeight - (maxHeight / 2);
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
                        } while (!isImportant(&pixels, x, y, maxWidth, maxHeight, importanceVarianceSize, level));
                        color temp = tempCamera.RandomSample(ren, tempCamera, ls, lCount, objs, objCount, tempCamera.pos.x, tempCamera.pos.y, sampleAmount, maxWidth, maxHeight, recursionMax, level * pixelSize, 0);
                        for (int u = 0; u < level * pixelSize; u++)
                        {
                            for (int v = 0; v < level * pixelSize; v++)
                            {
                                pixels[(((y + maxHeight / 2) + v) >= maxHeight ? maxHeight - 1 : (y + maxHeight / 2) + v) * maxHeight + (((x + maxWidth / 2) + u) >= maxWidth ? maxWidth - 1 : (x + maxWidth / 2) + u)] = temp;
                            }
                        }
                    }
                }
            }
            for (int x = 0; x < maxWidth; x++)
            {
                for (int y = 0; y < maxHeight; y++)
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
    case IMPORTANCE_JITTERED:
    {
        printf("Starting importance jittered\n");
        color *pixels = (color *)malloc(maxWidth * maxHeight * sizeof(color));
        for (int i = 0; i < maxWidth * maxHeight; i++)
        {
            pixels[i].r = 0;
            pixels[i].g = 0;
            pixels[i].b = 0;
        }
        for (int level = importanceStart; level > 0; level--)
        {
            printf("Starting level %d importance sample\n", level);
            for (int pass = 0; pass < passCount / level; pass++)
            {
                for (int ray = 0; ray < maxWidth / level; ray++)
                {
                    if (level >= generationEnd)
                    {
                        int x = ((rand() / (float)RAND_MAX) * maxWidth - maxWidth / 2) + (rand() / (float)RAND_MAX) * pixelSize * level;
                        int y = ((rand() / (float)RAND_MAX) * maxHeight - maxHeight / 2) + (rand() / (float)RAND_MAX) * pixelSize * level;
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
                        color temp = tempCamera.RandomSample(ren, tempCamera, ls, lCount, objs, objCount, x + (rand() / (float)RAND_MAX) * pixelSize * level, y + (rand() / (float)RAND_MAX) * pixelSize * level, sampleAmount, maxWidth, maxHeight, recursionMax, level * pixelSize, 0);
                        for (int u = 0; u < level * pixelSize; u++)
                        {
                            for (int v = 0; v < level * pixelSize; v++)
                            {
                                pixels[(((y + maxHeight / 2) + v) >= maxHeight ? maxHeight - 1 : (y + maxHeight / 2) + v) * maxHeight + (((x + maxWidth / 2) + u) >= maxWidth ? maxWidth - 1 : (x + maxWidth / 2) + u)] = temp;
                            }
                        }
                    }
                    else
                    {
                        cam tempCamera;
                        int x = 0;
                        int y = 0;
                        do
                        {
                            tempCamera = *this;
                            x = (rand() / (float)RAND_MAX) * maxWidth - (maxWidth / 2) + (rand() / (float)RAND_MAX) * pixelSize * level;
                            y = (rand() / (float)RAND_MAX) * maxHeight - (maxHeight / 2) + (rand() / (float)RAND_MAX) * pixelSize * level;
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
                        } while (!isImportant(&pixels, x, y, maxWidth, maxHeight, importanceVarianceSize, level));
                        color temp = tempCamera.RandomSample(ren, tempCamera, ls, lCount, objs, objCount, tempCamera.pos.x + (rand() / (float)RAND_MAX) * pixelSize * level, tempCamera.pos.y + (rand() / (float)RAND_MAX) * pixelSize * level, sampleAmount, maxWidth, maxHeight, recursionMax, level * pixelSize, 0);
                        for (int u = 0; u < level * pixelSize; u++)
                        {
                            for (int v = 0; v < level * pixelSize; v++)
                            {
                                pixels[(((y + maxHeight / 2) + v) >= maxHeight ? maxHeight - 1 : (y + maxHeight / 2) + v) * maxHeight + (((x + maxWidth / 2) + u) >= maxWidth ? maxWidth - 1 : (x + maxWidth / 2) + u)] = temp;
                            }
                        }
                    }
                }
            }
            for (int x = 0; x < maxWidth; x++)
            {
                for (int y = 0; y < maxHeight; y++)
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

bool cam::isImportant(color **pixels, int x, int y, int maxWidth, int maxHeight, int importanceVarianceSize, int level)
{
    for (int xx = -importanceVarianceSize * level; xx <= importanceVarianceSize * level; xx++)
    {
        for (int yy = -importanceVarianceSize * level; yy <= importanceVarianceSize * level; yy++)
        {
            if (((*pixels)[((y + yy + maxHeight / 2) >= maxHeight ? maxHeight - 1 : y + maxHeight / 2) * maxHeight + ((x + xx + maxWidth / 2) >= maxWidth ? maxWidth - 1 : x + maxWidth / 2)]).notBlack())
            {
                return true;
            }
        }
    }
    return false;
}

color cam::LinearSample(SDL_Renderer *ren, cam c, light *ls, int lCount, geo *objs, int objCount, int x, int y, float sampleAmount, int maxWidth, int maxHeight, int recursionMax, int pixelSize, bool draw)
{
    color pixel = color();

    for (float u = 0.0; u < pixelSize; u += (1 / sampleAmount) * pixelSize)
    {
        for (float v = 0.0; v < pixelSize; v += (1 / sampleAmount) * pixelSize)
        {
            cam tempCam = c;
            tempCam.pos.x += u;
            tempCam.pos.y += v;
            hit h = TraceObjs(tempCam, objs, objCount);
            if (h.hitSuccess)
            {
                color temp = h.obj->getMat()->shade(tempCam, ls, lCount, h, objs, objCount, 0, recursionMax);
                pixel.r += temp.r;
                pixel.g += temp.g;
                pixel.b += temp.b;
            }
            else
            {
            }
        }
    }
    pixel.r = pixel.r / (sampleAmount * sampleAmount);
    pixel.g = pixel.g / (sampleAmount * sampleAmount);
    pixel.b = pixel.b / (sampleAmount * sampleAmount);
    // printf("Pixel (%d, %d) color (%f, %f, %f)\n", x, y, pixel.r, pixel.g, pixel.b);
    pixel.normaliseF();
    // printf("Drawing Colour (%f, %f, %f) at (%d, %d)\n", pixel.r, pixel.g, pixel.b, x, y);
    // fflush(stdout);
    if (draw)
    {
        SDL_SetRenderDrawColor(ren, pixel.r * 255, pixel.g * 255, pixel.b * 255, 255);
        for (int i = 0; i < pixelSize; i++)
        {
            for (int j = 0; j < pixelSize; j++)
            {
                SDL_RenderDrawPoint(ren, x + i + (maxWidth / 2), y + j + (maxHeight / 2));
            }
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

color cam::RandomSample(SDL_Renderer *ren, cam c, light *ls, int lCount, geo *objs, int objCount, int x, int y, float sampleAmount, int maxWidth, int maxHeight, int recursionMax, int pixelSize, bool drawHere)
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
            output[i] = h.obj->getMat()->shade(tempCam, ls, lCount, h, objs, objCount, 0, recursionMax);
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
    if (drawHere)
    {
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
    }
    // printf("Returning pixel\n");
    return pixel;
}

void cam::PathTracePixel(SDL_Renderer *ren, cam c, light *ls, int lCount, geo *objs, int objCount, int x, int y, float sampleAmount, int maxWidth, int maxHeight, int recursionMax, int pixelSize)
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

color cam::PathTrace(SDL_Renderer *ren, cam c, light *ls, int lCount, geo *objs, int objCount, int maxWidth, int maxHeight, int recursionMax, int recursionCount)
{
    // printf("Tracing Path\n");
    // fflush(stdout);
    hit h = TraceObjs(c, objs, objCount);
    c.dir = c.dir.randomRay();
    if (!h.hitSuccess)
    {
        return color(0, 0, 0);
    }
    color output = h.obj->getMat()->shade(c, ls, lCount, h, objs, objCount, 0, -1);
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

hit cam::TraceObjs(cam c, geo *objs, int objCount)
{
    printf("Tracing Objects\n");
    hit bestHit;
    bestHit.dist = 100000000000000;
    for (int i = 0; i < objCount; i++)
    {
        hit h = objs[i].testRay(c.pos, c.dir);
        if (h.hitSuccess && h.dist < bestHit.dist)
        {
            printf("Hit Object\n");
            bestHit = h;
        }
    }
    printf("Traced Objects\n");
    return bestHit;
}