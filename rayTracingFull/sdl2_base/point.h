#include "vec.h"
class point {
    public:
        float x;
        float y;
        float z;

        point(float x, float y, float z);
        point();

        point add(point i);
        vec sub(point i);
        point mult(point i);
        float dp(point i);
        point cp(point i);

};