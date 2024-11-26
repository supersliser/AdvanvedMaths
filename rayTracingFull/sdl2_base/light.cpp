#include "light.h"
#include "vec.h"

light::light(point pos, color col, float brightness)
{
    this->pos = pos;
    this->col = col;
    this->brightness = brightness;
}

light::light() {
    this->pos.x = 0;
    this->pos.y = 0;
    this->pos.z = 0;
    this->col.r = 0;
    this->col.g = 0;
    this->col.b = 0;
    this->brightness = 0;
}

bool light::calculateShadows(hit h, sphere *sphereObjs, int sphereCount) {
    vec lDir;
    lDir = this->pos.sub(h.P);
    lDir = Normalise(lDir);
    for (int i = 0; i < sphereCount; i++) {
        hit temp = traceObj(h.P, lDir, sphereObjs[i]);
        if (temp.hit == 1 && temp.dist < h.dist && temp.obj.id != h.obj.id) {
            return 1;
        }
    }
    return 0;
}