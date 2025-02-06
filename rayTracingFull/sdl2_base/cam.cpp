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

void cam::renderFrames(SDL_Renderer *ren, light *l, int lCount, sphere *objs, int objCount, int maxWidth, int maxHeight, float sampleAmount, int recursionMax, bool progressive, SampleType sampleType, int passCount, int pixelSize, int importanceStart, int generationEnd, int importanceVarianceSize, int start, int end)
{
    for (int i = start; i < end; i++)
    {
        char frameName[100];
        sprintf(frameName, "frame%d.bmp", i);
        this->draw(ren, l, lCount, objs, objCount, maxWidth, maxHeight, sampleAmount, recursionMax, progressive, sampleType, passCount, pixelSize, importanceStart, generationEnd, importanceVarianceSize, i);
        SDL_Surface *sectionSurface = SDL_CreateRGBSurface(0, 1000, 1000, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
        SDL_RenderReadPixels(ren, NULL, SDL_PIXELFORMAT_RGBA8888, sectionSurface->pixels, sectionSurface->pitch);
        SDL_SaveBMP(sectionSurface, frameName);
        SDL_FreeSurface(sectionSurface);
    }
}

void cam::RandomShrinkingPixels(SDL_Renderer *ren, cam c, light *ls, int lCount, sphere *objs, int objCount, int maxWidth, int maxHeight, int recursionMax, int pixelSize, int passCount, int importanceStart, int generationEnd, int importanceVarianceSize, bool progressive, int sampleAmount, int level, int t)
{
    int x = (rand() / (float)RAND_MAX) * maxWidth - maxWidth / 2;
    int y = (rand() / (float)RAND_MAX) * maxHeight - maxHeight / 2;
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
    color temp = tempCamera.RandomSample(ren, tempCamera, ls, lCount, objs, objCount, x + (rand() / (float)RAND_MAX) * pixelSize * level, y + (rand() / (float)RAND_MAX) * pixelSize * level, sampleAmount, maxWidth, maxHeight, recursionMax, level * pixelSize, 0, t);
    // printf("Sampled successfully, applying sample to array\n");
    for (int u = 0; u < level * pixelSize; u++)
    {
        for (int v = 0; v < level * pixelSize; v++)
        {
            SDL_SetRenderDrawColor(ren, temp.r * 255, temp.g * 255, temp.b * 255, 255);
            SDL_RenderDrawPoint(ren, SDL_clamp(x + u + (maxWidth / 2), 0, maxWidth), SDL_clamp(y + v + (maxHeight / 2), 0, maxHeight));
        }
    }
}

void cam::draw(SDL_Renderer *ren, light *ls, int lCount, sphere *objs, int objCount, int maxWidth, int maxHeight, float sampleAmount, int recursionMax, bool progressive, SampleType sampleType, int passCount, int pixelSize, int importanceStart, int generationEnd, int importanceVarianceSize, int t)
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
                switch (sampleType)
                {
                case LINEAR:
                default:
                    tempCamera.LinearSample(ren, tempCamera, ls, lCount, objs, objCount, x, y, sampleAmount, maxWidth, maxHeight, recursionMax, pixelSize, 1, t);
                    break;
                }
                // printf("Sampled (%d, %d)\n", x, y);
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
                // printf("Starting Sample\n");
                fflush(stdout);
                switch (sampleType)
                {
                case RANDOM:
                default:
                    // tempCamera.printCam();
                    tempCamera.RandomSample(ren, tempCamera, ls, lCount, objs, objCount, x, y, sampleAmount, maxWidth, maxHeight, recursionMax, pixelSize, 1, t);
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
        for (int level = importanceStart; level > 0; level-=importanceVarianceSize)
        {
            // std::vector<std::thread> threads;
            if (level % 5 == 0 || level < 5) {
            printf("Starting level %d importance sample for frame %d\n", level, t);
            }
            for (int pass = 0; pass < passCount / level; pass++)
            {
                // threads.push_back(std::thread([=]()
                //                               { RandomShrinkingPixels(ren, *this, ls, lCount, objs, objCount, maxWidth, maxHeight, recursionMax, pixelSize, passCount, importanceStart, generationEnd, importanceVarianceSize, progressive, sampleAmount, level); }));
                // threads.back().join();

                RandomShrinkingPixels(ren, *this, ls, lCount, objs, objCount, maxWidth, maxHeight, recursionMax, pixelSize, passCount, importanceStart, generationEnd, importanceVarianceSize, progressive, sampleAmount, level, t);
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
                    color temp = tempCamera.RandomSample(ren, tempCamera, ls, lCount, objs, objCount, x, y, sampleAmount, maxWidth, maxHeight, recursionMax, level * pixelSize, 0, t);
                    // printf("Sampled successfully, applying sample to array\n");
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
        // printf("starting draw\n");
        for (int level = importanceStart; level > 0; level--)
        {
            printf("Starting level %d importance sample\n", level);
            {
                int x = (rand() / (float)RAND_MAX) * maxWidth - maxWidth / 2;
                int y = (rand() / (float)RAND_MAX) * maxHeight - maxHeight / 2;
                // printf("Starting sample at (%d, %d)\n", x, y);
                for (int ray = 0; ray < maxWidth / level; ray++)
                {
                    if (level >= generationEnd)
                    {
                        int x = (rand() / (float)RAND_MAX) * maxWidth - maxWidth / 2;
                        int y = (rand() / (float)RAND_MAX) * maxHeight - maxHeight / 2;
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
                        color temp = tempCamera.RandomSample(ren, tempCamera, ls, lCount, objs, objCount, x, y, sampleAmount, maxWidth, maxHeight, recursionMax, level * pixelSize, 0, t);
                        // printf("Sampled successfully, applying sample to array\n");
                        for (int u = 0; u < level * pixelSize; u++)
                        {
                            for (int v = 0; v < level * pixelSize; v++)
                            {
                                // printf("%d\n", (((y + maxHeight / 2) + v) > maxHeight ? maxHeight - 1 : (y + maxHeight / 2) + v) * maxHeight + (x + maxWidth / 2) + u);
                                pixels[(((y + maxHeight / 2) + v) >= maxHeight ? maxHeight - 1 : (y + maxHeight / 2) + v) * maxHeight + (((x + maxWidth / 2) + u) >= maxWidth ? maxWidth - 1 : (x + maxWidth / 2) + u)] = temp;
                            }
                        }

                        // printf("Sampled (%d, %d)\n", x, y);
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
                            // printf("x: %d, y: %d\n", x, y);
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
                            // printf("Sampling (%d, %d)\n", tempCamera.pos.x + 500, tempCamera.pos.y + 500);
                            // printf("%d\n", (int)floor((tempCamera.pos.x + 500) * maxWidth + (tempCamera.pos.y + 500)));
                            // printf("Pixel color (%f, %f, %f) at (%d, %d)\n", pixels[(int)floor((tempCamera.pos.x + 500) * maxWidth + (tempCamera.pos.y + 500))].r, pixels[(int)floor((tempCamera.pos.x + 500) * maxWidth + (tempCamera.pos.y + 500))].g, pixels[(int)floor((tempCamera.pos.x + 500) * maxWidth + (tempCamera.pos.y + 500))].b, x + 500, y + 500);
                        } while (!pixels[((y + maxHeight / 2) >= maxHeight ? maxHeight - 1 : y + maxHeight / 2) * maxHeight + ((x + maxWidth / 2) >= maxWidth ? maxWidth - 1 : x + maxWidth / 2)].notBlack());
                        // printf("Found not black\n");
                        color temp = tempCamera.RandomSample(ren, tempCamera, ls, lCount, objs, objCount, tempCamera.pos.x, tempCamera.pos.y, sampleAmount, maxWidth, maxHeight, recursionMax, level * pixelSize, 0, t);
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
        // printf("starting draw\n");
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
                        color temp = tempCamera.RandomSample(ren, tempCamera, ls, lCount, objs, objCount, x, y, sampleAmount, maxWidth, maxHeight, recursionMax, level * pixelSize, 0, t);
                        // printf("Sampled successfully, applying sample to array\n");
                        for (int u = 0; u < level * pixelSize; u++)
                        {
                            for (int v = 0; v < level * pixelSize; v++)
                            {
                                // printf("%d\n", (((y + maxHeight / 2) + v) > maxHeight ? maxHeight - 1 : (y + maxHeight / 2) + v) * maxHeight + (x + maxWidth / 2) + u);
                                pixels[(((y + maxHeight / 2) + v) >= maxHeight ? maxHeight - 1 : (y + maxHeight / 2) + v) * maxHeight + (((x + maxWidth / 2) + u) >= maxWidth ? maxWidth - 1 : (x + maxWidth / 2) + u)] = temp;
                            }
                        }

                        // printf("Sampled (%d, %d)\n", x, y);
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
                            // printf("x: %d, y: %d\n", x, y);
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
                            // printf("Sampling (%d, %d)\n", tempCamera.pos.x + 500, tempCamera.pos.y + 500);
                            // printf("%d\n", (int)floor((tempCamera.pos.x + 500) * maxWidth + (tempCamera.pos.y + 500)));
                            // printf("Pixel color (%f, %f, %f) at (%d, %d)\n", pixels[(int)floor((tempCamera.pos.x + 500) * maxWidth + (tempCamera.pos.y + 500))].r, pixels[(int)floor((tempCamera.pos.x + 500) * maxWidth + (tempCamera.pos.y + 500))].g, pixels[(int)floor((tempCamera.pos.x + 500) * maxWidth + (tempCamera.pos.y + 500))].b, x + 500, y + 500);
                        } while (!isImportant(&pixels, x, y, maxWidth, maxHeight, importanceVarianceSize, level));
                        // printf("Found not black\n");
                        color temp = tempCamera.RandomSample(ren, tempCamera, ls, lCount, objs, objCount, tempCamera.pos.x, tempCamera.pos.y, sampleAmount, maxWidth, maxHeight, recursionMax, level * pixelSize, 0, t);
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
        // printf("starting draw\n");
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
                        color temp = tempCamera.RandomSample(ren, tempCamera, ls, lCount, objs, objCount, x + (rand() / (float)RAND_MAX) * pixelSize * level, y + (rand() / (float)RAND_MAX) * pixelSize * level, sampleAmount, maxWidth, maxHeight, recursionMax, level * pixelSize, 0, t);
                        // printf("Sampled successfully, applying sample to array\n");
                        for (int u = x; u < x + level * pixelSize; u+1 >= maxWidth ? u = maxWidth - 1 : u++)
                        {
                            for (int v = y; v < y + level * pixelSize; v+1 >= maxHeight ? v = maxHeight - 1 : v++)
                            {
                                // printf("%d\n", (((y + maxHeight / 2) + v) > maxHeight ? maxHeight - 1 : (y + maxHeight / 2) + v) * maxHeight + (x + maxWidth / 2) + u);
                                pixels[y * maxHeight + x] = temp;
                            }
                        }

                        // printf("Sampled (%d, %d)\n", x, y);
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
                            // printf("x: %d, y: %d\n", x, y);
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
                            // printf("Sampling (%d, %d)\n", tempCamera.pos.x + 500, tempCamera.pos.y + 500);
                            // printf("%d\n", (int)floor((tempCamera.pos.x + 500) * maxWidth + (tempCamera.pos.y + 500)));
                            // printf("Pixel color (%f, %f, %f) at (%d, %d)\n", pixels[(int)floor((tempCamera.pos.x + 500) * maxWidth + (tempCamera.pos.y + 500))].r, pixels[(int)floor((tempCamera.pos.x + 500) * maxWidth + (tempCamera.pos.y + 500))].g, pixels[(int)floor((tempCamera.pos.x + 500) * maxWidth + (tempCamera.pos.y + 500))].b, x + 500, y + 500);
                        } while (!isImportant(&pixels, x, y, maxWidth, maxHeight, importanceVarianceSize, level));
                        // printf("Found not black\n");
                        color temp = tempCamera.RandomSample(ren, tempCamera, ls, lCount, objs, objCount, tempCamera.pos.x + (rand() / (float)RAND_MAX) * pixelSize * level, tempCamera.pos.y + (rand() / (float)RAND_MAX) * pixelSize * level, sampleAmount, maxWidth, maxHeight, recursionMax, level * pixelSize, 0, t);
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

color cam::LinearSample(SDL_Renderer *ren, cam c, light *ls, int lCount, sphere *objs, int objCount, int x, int y, float sampleAmount, int maxWidth, int maxHeight, int recursionMax, int pixelSize, bool draw, int t)
{
    // printf("Sampling (%d, %d)\n", x, y);
    color pixel = color();

    for (float u = 0.0; u < pixelSize; u += (1 / sampleAmount) * pixelSize)
    {
        for (float v = 0.0; v < pixelSize; v += (1 / sampleAmount) * pixelSize)
        {
            cam tempCam = c;
            tempCam.pos.x += u;
            tempCam.pos.y += v;
            hit h = TraceObjs(tempCam, objs, objCount, t);
            // printf("Sampling (%f, %f)\n", x, y);
            if (h.hitSuccess)
            {
                // printf("Hit success at (%f, %f)\n", h->P.x, h->P.y);
                // fflush(stdout);
                color temp = h.obj->m->shade(tempCam, ls, lCount, h, objs, objCount, 0, recursionMax, t);
                pixel.r += temp.r;
                pixel.g += temp.g;
                pixel.b += temp.b;
            }
            else
            {
            }
            // printf("Sampling (%f, %f)\n", x, y);
            // printf("Sample (%f, %f) color (%f, %f, %f)\n", u, v, output[(int)(u * sampleAmount)][(int)(v * sampleAmount)].r, output[(int)(u * sampleAmount)][(int)(v * sampleAmount)].g, output[(int)(u * sampleAmount)][(int)(v * sampleAmount)].b);
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

color cam::RandomSample(SDL_Renderer *ren, cam c, light *ls, int lCount, sphere *objs, int objCount, int x, int y, float sampleAmount, int maxWidth, int maxHeight, int recursionMax, int pixelSize, bool drawHere, int t)
{
    color output[(int)sampleAmount];
    for (int i = 0; i < sampleAmount; i++)
    {
        cam tempCam = c;
        tempCam.pos.x += (float)rand() / (float)RAND_MAX;
        tempCam.pos.y += (float)rand() / (float)RAND_MAX;
        hit h = TraceObjs(tempCam, objs, objCount, t);
        // printf("Traced objects\n");
        if (h.hitSuccess)
        {
            output[i] = h.obj->m->shade(tempCam, ls, lCount, h, objs, objCount, 0, recursionMax, t);
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

hit cam::TraceObjs(cam c, sphere *objs, int objCount, int t)
{
    hit bestHit;
    bestHit.dist = 100000000000000;
    // printf("%d\n", objCount);
    for (int i = 0; i < objCount; i++)
    {
        hit h = objs[i].traceObj(c.pos, c.dir, t);
        if (h.hitSuccess && h.dist < bestHit.dist)
        {
            bestHit = h;
        }
    }
    return bestHit;
}