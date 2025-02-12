#include "geo.h"
#include <cstdlib> // for free

face::face()
{
    this->vertices = new vertex *[0];
    this->vertexCount = 0;
}

face::face(vertex *v1, vertex *v2, vertex *v3)
{
    this->vertices = (vertex **)calloc(3, sizeof(vertex*));
    this->vertices[0] = v1;
    this->vertices[1] = v2;
    this->vertices[2] = v3;

    v1->addFace(this);
    v2->addFace(this);
    v3->addFace(this);

    this->vertexCount = 3;
    printf("Face Created\n");
}

// face::face(vertex** vs, int vertexCount) {
//     this->vertices = vs;
//     this->vertexCount = vertexCount;
// }

vec face::calculateNormal() {
    if (this->normal.x != 0 || this->normal.y != 0 || this->normal.z != 0)
    {
        return this->normal;
    }
    vec v1 = this->vertices[1]->getPos().sub(this->vertices[0]->getPos());
    vec v2 = this->vertices[2]->getPos().sub(this->vertices[0]->getPos());
    this->normal = v1.cp(v2).Normalise();
    return this->normal;
}

int face::getVertexCount()
{
    return this->vertexCount;
}

void face::addVertex(vertex *v)
{
    vertex **temp = (vertex **)calloc(this->vertexCount + 1, sizeof(vertex*));
    for (int i = 0; i < vertexCount; i++)
    {
        if (this->vertices[i] == v)
        {
            return;
        }
        temp[i] = this->vertices[i];
    }
    v->addFace(this);
    temp[vertexCount] = v;
    free(this->vertices);
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

void face::printFace()
{
    printf("Face Pointer: %p\n", this);
    printf("Vertex Count: %d\n", this->vertexCount);
    printf("Vertices: \n");
    for (int i = 0; i < vertexCount; i++)
    {
        printf("Vertex %d: %p\n", i, this->vertices[i]);
    }
    vec n = this->calculateNormal();
    printf("Normal: (%f, %f, %f)\n", n.x, n.y, n.z);
    printf("\n");
}

vec face::getNormal()
{
    return this->normal;
}

void face::setNormal(vec n)
{
    this->normal = n;
}

face::~face()
{
    if (this->vertices != nullptr)
    {
        free(this->vertices);
    }
}

