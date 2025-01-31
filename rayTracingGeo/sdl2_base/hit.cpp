#include "hit.h"

hit::hit()
{
    this->hitSuccess = 0;
}



hit::hit(bool h, geo* obj, float dist, point P, face* f)
{
    this->hitSuccess = h;
    this->obj = obj;
    this->dist = dist;
    this->P = P;
    this->f = f;
}