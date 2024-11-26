#include "hit.h"

hit::hit()
{
    this->hit = 0;
}



hit::hit(char h, sphere obj, float dist, point P)
{
    this->hit = h;
    this->obj = obj;
    this->dist = dist;
    this->P = P;
}