#include "vertex.h"

vertex::vertex()
{
    this->x = 0;
    this->y = 0;
    this->z = 0;
    this->connections = nullptr;
    this->connectionCount = 0;
    this->faces = nullptr;
    this->faceCount = 0;
}

vertex::vertex(float x, float y, float z)
{
    this->x = x;
    this->y = y;
    this->z = z;
    this->connections = nullptr;
    this->faces = nullptr;
    this->faceCount = 0;
    this->connectionCount = 0;
}

void vertex::addConnection(vertex *v)
{
    if (v == NULL || v == this || v == nullptr)
    {
        return;
    }
    vertex **temp = (vertex **)calloc(this->connectionCount + 1, sizeof(vertex*));
    for (int i = 0; i < this->connectionCount; i++)
    {
        if (this->connections[i] == v)
        {
            return;
        }
        temp[i] = this->connections[i];
    }
    temp[this->connectionCount] = v;
    free(this->connections);
    this->connections = temp;
    this->connectionCount++;
}

void vertex::removeConnection(vertex *v)
{
    vertex *temp[this->connectionCount + 1];
    for (int i = 0; i < this->connectionCount; i++)
    {
        if (this->connections[i] == v)
        {
            continue;
        }
        temp[i] = this->connections[i];
    }
        free(this->connections);
    this->connections = temp;
    this->connectionCount--;
}

vertex **vertex::getConnections()
{
    return this->connections;
}

int vertex::getConnectionIndex(vertex *v)
{
    for (int i = 0; i < this->connectionCount; i++)
    {
        if (this->connections[i] == v)
        {
            return i;
        }
    }
    return -1;
}

int vertex::getConnectionCount()
{
    return this->connectionCount;
}

vertex *vertex::getConnection(int i)
{
    return this->connections[i];
}

int vertex::getFaceCount()
{
    return this->faceCount;
}

void vertex::addFace(face *f)
{
    face **temp = (face **)calloc(this->faceCount + 1, sizeof(face*));
    for (int i = 0; i < this->faceCount; i++)
    {
        if (this->faces[i] == f)
        {
            return;
        }
        temp[i] = this->faces[i];
    }
    temp[this->faceCount] = f;
    free(this->faces);
    this->faces = temp;
    this->faceCount++;
}

void vertex::removeFace(face *f)
{
    face *temp[this->faceCount + 1];
    for (int i = 0; i < this->faceCount; i++)
    {
        if (this->faces[i] == f)
        {
            continue;
        }
        temp[i] = this->faces[i];
    }
    this->faces = temp;
    this->faceCount--;
}

face **vertex::getFaces()
{
    return this->faces;
}

int vertex::getFaceIndex(face *f)
{
    for (int i = 0; i < this->faceCount; i++)
    {
        if (this->faces[i] == f)
        {
            return i;
        }
    }
    return -1;
}

face *vertex::getFace(int i)
{
    return this->faces[i];
}

bool vertex::operator==(vertex v)
{
    return this->x == v.x && this->y == v.y && this->z == v.z;
}

bool vertex::operator!=(vertex v)
{
    return this->x != v.x || this->y != v.y || this->z != v.z;
}

void vertex::printVertex()
{
    printf("Vertex pointer: %p\n", this);
    printf("Vertex Connections: %d\n", this->connectionCount);
    for (int i = 0; i < this->connectionCount; i++)
    {
        printf("Connection %d: %p\n", i, this->connections[i]);
    }
    printf("Vertex Faces: %d\n", this->faceCount);
    for (int i = 0; i < this->faceCount; i++)
    {
        printf("Face %d: %p\n", i, this->faces[i]);
    }
    printf("Vertex Data: (%f, %f, %f)\n", this->x, this->y, this->z);
    fflush(stdout);
}

point vertex::getPos()
{
    return point(this->x, this->y, this->z);
}