#include "light.h"
#include <cstdlib> // for free

light::light(point pos, color col, float brightness)
{
    this->pos = pos;
    this->col = col;
    this->brightness = brightness;
}

light::light()
{
    this->pos.x = 0;
    this->pos.y = 0;
    this->pos.z = 0;
    this->col.r = 0;
    this->col.g = 0;
    this->col.b = 0;
    this->brightness = 0;
}

light::~light()
{
    // No dynamic memory to free in light class
}

bool light::calculateShadows(hit th, geo *objs, int objCount)
{
    vec lDir;
    lDir = this->pos.sub(th.P);
    lDir = lDir.Normalise();
    hit h = hit();
    h.dist = MAXFLOAT;
    for (int i = 0; i < objCount; i++)
    {
        int faceCount;
        face **faces = objs[i].getFaces(&faceCount);
        for (int j = 0; j < faceCount; j++)
        {
            hit temp = objs[i].testRay(h.P, lDir, faces[j]);
            if (temp.hitSuccess && temp.dist < h.dist)
            {
                h = temp;
            }
        }
    }
    if (h.hitSuccess)
    {
        return true;
    }
    return 0;
}