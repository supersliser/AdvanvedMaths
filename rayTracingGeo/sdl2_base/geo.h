#pragma once

#include "vec.h"
#include "mat.h"
#include "vertex.h"
#include "hit.h"

class vec;
class mat;
class vertex;
class hit;

class face {
    private:
        vertex** vertices;
        int vertexCount;
        vec normal;
    public:
        face();
        face(vertex* v1, vertex* v2, vertex* v3);
        face(vertex**vs, int vertexCount);
        void addVertex(vertex* v);
        void removeVertex(vertex* v);
        vertex** getVertices();
        vec getNormal();
};

class geo {
    private:
        int vertexCount;
        vertex** vertices;
        point center;
        vec translation;
        vec rotation;
        vec scale;
        mat *m;

        face** getFaces(int*gFaceCount);
    public:
        geo();
        geo(point c);
        geo(point c, mat *m);
        void addVertex(vertex* v);
        void removeVertex(vertex* v);
        void translate(vec v);
        void rotate(vec v);
        void scaleSize(vec v);
        hit testRay(point p, vec v);
        vertex** getVertices();
        point getCenter();
        vec getTranslation();
        vec getRotation();
        vec getScale();
        vec getNormal(point p);
        mat* getMat();
        void setMat(mat* m);
        void makeFace(vertex** vs, int vertexCount);
        void addVertices(vertex** v, int count);
};
