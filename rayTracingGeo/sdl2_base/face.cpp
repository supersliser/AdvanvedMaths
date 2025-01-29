#include "geo.h"

face::face()
{
    this->vertices = new vertex *[0];
    this->normal = vec();
    this->vertexCount = 0;
}

face::face(vertex *v1, vertex *v2, vertex *v3)
{
    this->vertices = new vertex *[3];
    this->vertices[0] = v1;
    this->vertices[1] = v2;
    this->vertices[2] = v3;
    vec v1v2 = vec(v1->x - v2->x, v1->y - v2->y, v1->z - v2->z);
    vec v1v3 = vec(v1->x - v3->x, v1->y - v3->y, v1->z - v3->z);
    this->normal = v1v2.cp(v1v3);
    this->normal.Normalise();

    v1->addFace(this);
    v2->addFace(this);
    v3->addFace(this);

    this->vertexCount = 3;
}

face::face(vertex**vs, int vertexCount) {
    this->vertices = vs;
    vec v1v2 = vec(vs[0]->x - vs[1]->x, vs[0]->y - vs[1]->y, vs[0]->z - vs[1]->z);
    vec v1v3 = vec(vs[0]->x - vs[2]->x, vs[0]->y - vs[2]->y, vs[0]->z - vs[2]->z);
    this->normal = v1v2.cp(v1v3);
        this->normal = v1v2.cp(v1v3);
    this->normal.Normalise();

    for (int i = 0; i < vertexCount; i++) {
        vs[i]->addFace(this);
    }
    this->vertexCount = vertexCount;
}

void face::addVertex(vertex *v)
{
    vertex *temp[vertexCount + 1];
    for (int i = 0; i < vertexCount; i++)
    {
        if (this->vertices[i] == v)
        {
            return;
        }
        temp[i] = this->vertices[i];
    }
    v->addFace(this);
    temp[vertexCount + 1] = v;
    this->vertices = temp;
    this->vertexCount++;
}

void face::removeVertex(vertex *v)
{
    vertex *temp[vertexCount + 1];
    for (int i = 0; i < vertexCount; i++)
    {
        if (this->vertices[i] == v)
        {
            continue;
        }
        temp[i] = this->vertices[i];
    }
    this->vertices = temp;
    this->vertexCount--;
}

vertex **face::getVertices()
{
    return this->vertices;
}

vec face::getNormal()
{
    return this->normal;
}