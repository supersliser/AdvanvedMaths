//
// Created by t on 07/07/25.
//
#include <algorithm>
#include <tgmath.h>
#include <SDL2/SDL.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <fstream>
#include <oneapi/tbb/detail/_utils.h>

// Structure to represent a point in 3D space
typedef struct point
{
    float x;
    float y;
    float z;
} point;

// Structure to represent a vector in 3D space
typedef struct vec
{
    float x;
    float y;
    float z;
} vec;

// Structure to represent a color in RGB format
typedef struct color
{
    float r;
    float g;
    float b;
} color;

// Structure to represent a triangle in 3D space
typedef struct triangle
{
    point v1;
    point v2;
    point v3;
    color col;
} triangle;

// Structure to represent a hit result from ray tracing
typedef struct hit
{
    bool didHit;
    triangle obj;
    float dist;
    point P;
} hit;

// Structure to represent a camera in 3D space
typedef struct cam
{
    point pos;
    vec dir;
} cam;

// Function to calculate the dot product of two points
float dp(point p1, point p2)
{
    return p1.x * p2.x + p1.y * p2.y + p1.z * p2.z;
}

// Function to calculate the dot product of a vector and a point
float dp(vec p1, point p2)
{
    return p1.x * p2.x + p1.y * p2.y + p1.z * p2.z;
}

// Function to calculate the dot product of two vectors
float dp(vec p1, vec p2)
{
    return p1.x * p2.x + p1.y * p2.y + p1.z * p2.z;
}

// Function to normalize a vector
vec Normalise(vec v)
{
    float length = sqrt(dp(v, v));
    v.x /= length;
    v.y /= length;
    v.z /= length;
    return v;
}

// Function to calculate the normal vector of a triangle
vec getNormal(triangle obj)
{
    vec ca = {obj.v3.x - obj.v1.x, obj.v3.y - obj.v1.y, obj.v3.z - obj.v1.z};
    vec cb = {obj.v3.x - obj.v2.x, obj.v3.y - obj.v2.y, obj.v3.z - obj.v2.z};
    vec N;
    N.x = ca.y * cb.z - ca.z * cb.y;
    N.y = ca.z * cb.x - ca.x * cb.z;
    N.z = ca.x * cb.y - ca.y * cb.x;
    return Normalise(N);
}

// Function to trace a ray from a source point in a given direction against a triangle object
hit traceObj(point src, vec dir, triangle obj)
{
    hit out;
    out.didHit = 0;

    vec edge1, edge2, h, s, q;
    float a, f, u, v;

    edge1.x = obj.v2.x - obj.v1.x;
    edge1.y = obj.v2.y - obj.v1.y;
    edge1.z = obj.v2.z - obj.v1.z;

    edge2.x = obj.v3.x - obj.v1.x;
    edge2.y = obj.v3.y - obj.v1.y;
    edge2.z = obj.v3.z - obj.v1.z;

    h.x = dir.y * edge2.z - dir.z * edge2.y;
    h.y = dir.z * edge2.x - dir.x * edge2.z;
    h.z = dir.x * edge2.y - dir.y * edge2.x;

    a = dp(edge1, h);

    //if a = 0 with floating point innaccuracy consideration
    //if a did = 0 then it would cause a crash as it is used to divide with later on
    if (a > -0.000000001f && a < 0.000000001f)
    {
        return out;
    }

    f = 1.0f / a;

    s.x = src.x - obj.v1.x;
    s.y = src.y - obj.v1.y;
    s.z = src.z - obj.v1.z;

    u = f * dp(s, h);

    if (u < 0.0f || u > 1.0f)
    {
        return out;
    }

    q.x = s.y * edge1.z - s.z * edge1.y;
    q.y = s.z * edge1.x - s.x * edge1.z;
    q.z = s.x * edge1.y - s.y * edge1.x;

    v = f * dp(dir, q);

    if (v < 0.0f || u + v > 1.0f)
    {
        return out;
    }

    float t = f * dp(edge2, q);

    if (t > 0.000000001f) // Ray intersection
    {
        out.didHit = 1;
        out.dist = t;
        out.obj = obj;
        out.P.x = src.x + dir.x * t;
        out.P.y = src.y + dir.y * t;
        out.P.z = src.z + dir.z * t;
    }
    return out;
}

bool calculateShadows(hit h, triangle* triangles, int triangleCount, point l)
{
    //get the direction of the light relative to the collision point
    vec lDir;
    lDir = Normalise({l.x - h.P.x, l.y - h.P.y, l.z - h.P.z});
    //test for all other triangles
    for (int i = 0; i < triangleCount; i++)
    {
        hit temp = traceObj(h.P, lDir, triangles[i]);
        // if h.dist < 0.001f then its probably the same triangle
        if (temp.didHit && temp.dist < h.dist && h.dist > 0.001f)
        {
            return 1;
        }
    }
    return 0;
}

color shade(cam c, point l, hit h, triangle* triangles, int triangleCount)
{
    // diffusion
    color finalDiffuse;

    vec dlDir;
    dlDir.x = l.x - h.P.x;
    dlDir.y = l.y - h.P.y;
    dlDir.z = l.z - h.P.z;
    dlDir = Normalise(dlDir);
    float diff = dp(dlDir, getNormal(h.obj));
    if (diff < 0.0f)
    {
        diff = 0.0f;
    }
    if (diff > 1.0f)
    {
        diff = 1.0f;
    }
    finalDiffuse.r = diff * h.obj.col.r;
    finalDiffuse.g = diff * h.obj.col.g;
    finalDiffuse.b = diff * h.obj.col.b;
    // printf("%f, %f, %f\n", diffuse.r, diffuse.g, diffuse.b);

    bool sh = calculateShadows(h, triangles, triangleCount, l);

    color output;
    output.r = (finalDiffuse.r) * sh;
    output.g = (finalDiffuse.g) * sh;
    output.b = (finalDiffuse.b) * sh;
    output.r = std::clamp(output.r, 0.0f, 255.0f);
    output.g = std::clamp(output.g, 0.0f, 255.0f);
    output.b = std::clamp(output.b, 0.0f, 255.0f);
    // printf("%f, %f, %f\n", output.r, output.g, output.b);
    return output;
}


// Function to draw the scene by tracing rays from the camera position
void draw(SDL_Renderer* ren, cam c, triangle* objs, int objCount)
{
    //converting camera to true orthographic coords
    vec viewDir = Normalise(c.dir);
    vec right;
    vec up;
    if (fabs(viewDir.x) < fabs(viewDir.y) && fabs(viewDir.x) < fabs(viewDir.z))
    {
        right = {0, viewDir.z, -viewDir.y};
    }
    else
    {
        right = {-viewDir.z, 0, viewDir.x};
    }
    right = Normalise(right);

    up.x = viewDir.y * right.z - viewDir.z * right.y;
    up.y = viewDir.z * right.x - viewDir.x * right.z;
    up.z = viewDir.x * right.y - viewDir.y * right.x;
    up = Normalise(up);

    //must be same number in loop as there is number of pixels in image (1000x1000)
    //using 100.0x100.0 to simulate zoom to make object easier to see
    for (float x = -50; x < 50; x += 0.1f)
    {
        for (float y = -50; y < 50; y += 0.1f)
        {
            point pixelPosition;

            pixelPosition.x = c.pos.x + x * right.x + y * up.x;
            pixelPosition.y = c.pos.y + x * right.y + y * up.y;
            pixelPosition.z = c.pos.z + x * right.z + y * up.z;

            hit bestHit;
            bestHit.didHit = false;
            bestHit.dist = 100000000.0f;
            // test all objects, save the closest hit
            for (int i = 0; i < objCount; i++)
            {
                hit h = traceObj(pixelPosition, viewDir, objs[i]);
                if (h.didHit && h.dist < bestHit.dist)
                {
                    bestHit.dist = h.dist;
                    bestHit.didHit = 1;
                    bestHit.obj = h.obj;
                    bestHit.P = h.P;
                }
            }
            if (bestHit.didHit)
            {
                //draw the pixel
                // printf("%d, %d\n", x, y);
                cam temp = c;
                temp.pos.x = 100.0f;
                temp.pos.y = 0.0f;
                temp.pos.z = 100.0f;
                color col = shade(c, temp.pos, bestHit, objs, objCount);
                SDL_SetRenderDrawColor(ren, col.r, col.g, col.b, 255);
                SDL_RenderDrawPoint(ren, (x + 50) * 10, (y + 50) * 10);
            }
            else
            {
                // draw the background
                SDL_SetRenderDrawColor(ren, 0, 0, 128, 255);
                SDL_RenderDrawPoint(ren, (x + 50) * 10, (y + 50) * 10);
            }
        }
        printf("rendered %.2f\n", x);
    }
    SDL_RenderPresent(ren);
    fflush(stdout);
    printf("drawn\n");
}

int main(int argc, char* argv[])
{
    // returns zero on success else non-zero
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        printf("error initializing SDL: %s\n", SDL_GetError());
    }
    SDL_Window* win = SDL_CreateWindow("GAME",
                                       SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED,
                                       1000, 1000, 0);

    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, 0);
    SDL_SetRenderDrawColor(ren, 0, 128, 0, 255);
    SDL_RenderClear(ren);

    std::string file_location = "../ObjectFiles/objectHeavy.txt";
    // each line contains 9 floats representing a triangle
    // load each line in
    std::vector<triangle> objects;
    std::ifstream file(file_location);

    if (!file.is_open())
    {
        printf("Failed to open file: %s\n", file_location.c_str());
        return 1;
    }

    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        triangle triangle;

        // Parse first vertex
        char comma; // for reading and discarding commas
        iss >> triangle.v1.x >> comma >> triangle.v1.y >> comma >> triangle.v1.z;

        // Parse second vertex
        iss >> triangle.v2.x >> comma >> triangle.v2.y >> comma >> triangle.v2.z;

        // Parse third vertex
        iss >> triangle.v3.x >> comma >> triangle.v3.y >> comma >> triangle.v3.z;

        if (iss)
        {
            //the colour of each triangle is red
            triangle.col = {255, 0, 0};
            objects.push_back(triangle);
            printf("Triangle Loaded: (%f, %f, %f), (%f, %f, %f), (%f, %f, %f)\n",
                   triangle.v1.x, triangle.v1.y, triangle.v1.z,
                   triangle.v2.x, triangle.v2.y, triangle.v2.z,
                   triangle.v3.x, triangle.v3.y, triangle.v3.z);
        }
    }


    cam c;
    c.pos.x = 0;
    c.pos.y = 0;
    // -5000 to leave plenty of room to avoid culling of triangles (if they're behind the camera)
    c.pos.z = -5000;
    c.dir.x = 0;
    c.dir.y = 0;
    c.dir.z = 1;

    printf("Objects created\n");
    printf("Object count: %lu\n", objects.size());

    draw(ren, c, objects.data(), objects.size());


    // the main event loop
    char finished = 0;
    while (!finished)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                finished = 1;
                break;
            }
        }
    }

    SDL_Quit();

    return 0;
}
