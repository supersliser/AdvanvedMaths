#include "vec.h"
class color {
public:
    float r;
    float g;
    float b;

    color(float r, float g, float b);
    color();
    vec toVec();
    void toCol(vec i);
    void normaliseF();
    void normalise255();
};