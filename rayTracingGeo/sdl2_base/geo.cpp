#include "geo.h"

geo::geo()
{
    this->center = point();
    this->translation = vec();
    this->rotation = vec();
    this->scale = vec();
    this->m = new mat();
    this->vertices = nullptr;
    this->vertexCount = 0;
}

geo::geo(point c)
{
    this->center = c;
    this->translation = vec();
    this->rotation = vec();
    this->scale = vec();
    this->m = new mat();
    this->vertices = nullptr;
    this->vertexCount = 0;
}

geo::geo(point c, mat *m)
{
    this->center = c;
    this->translation = vec();
    this->rotation = vec();
    this->scale = vec();
    this->m = m;
    this->vertices = nullptr;
    this->vertexCount = 0;
}

void geo::addVertex(vertex *v)
{
    vertex *temp[this->vertexCount + 1];
    for (int i = 0; i < vertexCount; i++)
    {
        if (this->vertices[i] == v)
        {
            return;
        }
        temp[i] = this->vertices[i];
    }
    temp[this->vertexCount + 1] = v;
    this->vertices = temp;
    this->vertexCount++;
}

void geo::removeVertex(vertex *v)
{
    vertex *temp[this->vertexCount + 1];
    for (int i = 0; i < vertexCount; i++)
    {
        if (this->vertices[i] == v)
        {
            continue;
            ;
        }
        temp[i] = this->vertices[i];
    }
    this->vertices = temp;
    this->vertexCount--;
}

void geo::translate(vec v)
{
    this->translation = v;
}

void geo::rotate(vec v)
{
    this->rotation = v;
}

void geo::scaleSize(vec v)
{
    this->scale = v;
}

hit geo::testRay(point p, vec v)
{
    printf("Testing Ray\n");
    int faceCount;
    face **faces = this->getFaces(&faceCount);
    printf("Got Faces\n");

    for (int i = 0; i < faceCount; i++)
    {
        vertex **vertices = faces[i]->getVertices();
        printf("got vertices\n");
        fflush(stdout);
        vec normal = faces[i]->getNormal();
        printf("got normal\n");
        fflush(stdout);

        vec N = normal;
        vec L = vec(p.x - vertices[0]->x, p.y - vertices[0]->y, p.z - vertices[0]->z);
        float t = N.dp(L) / N.dp(v);
        if (t < 0)
        {
            continue;
        }
        point P = point(p.x + v.x * t, p.y + v.y * t, p.z + v.z * t);
        vec C;
        C.x = P.x - vertices[0]->x;
        C.y = P.y - vertices[0]->y;
        C.z = P.z - vertices[0]->z;
        vec A = vec(vertices[1]->x - vertices[0]->x, vertices[1]->y - vertices[0]->y, vertices[1]->z - vertices[0]->z);
        vec B = vec(vertices[2]->x - vertices[0]->x, vertices[2]->y - vertices[0]->y, vertices[2]->z - vertices[0]->z);
        vec D = vec(P.x - vertices[0]->x, P.y - vertices[0]->y, P.z - vertices[0]->z);
        float dot00 = A.dp(A);
        float dot01 = A.dp(B);
        float dot02 = A.dp(D);
        float dot11 = B.dp(B);
        float dot12 = B.dp(D);
        float invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
        float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
        float v = (dot00 * dot12 - dot01 * dot02) * invDenom;
        if (u >= 0 && v >= 0 && u + v <= 1)
        {
            return hit(true, this, t, P);
        }
    }
    return hit(false, this, 0, point());
}

vertex **geo::getVertices()
{
    return this->vertices;
}

point geo::getCenter()
{
    return this->center;
}

vec geo::getTranslation()
{
    return this->translation;
}

vec geo::getRotation()
{
    return this->rotation;
}

vec geo::getScale()
{
    return this->scale;
}

vec geo::getNormal(point p)
{
    return vec();
}

mat *geo::getMat()
{
    return this->m;
}

void geo::setMat(mat *m)
{
    this->m = m;
}

void geo::makeFace(vertex **vs, int vertexCount)
{
    for (int i = 0; i < vertexCount; i++)
    {
        for (int j = 0; j < vertexCount; j++)
        {
            if (i != j)
            {
                printf("Connecting %p to %p\n", vs[i], vs[j]);
                vs[i]->addConnection(vs[j]);
            }
        }
    }

    face *temp = new face(vs, vertexCount);

    for (int i = 0; i < vertexCount; i++)
    {
        vs[i]->addFace(temp);
        vs[i]->printVertex();
    }
}

face **geo::getFaces(int *gFaceCount)
{
    face **faces = nullptr;
    int faceCount = 0;
    printf("%d\n", this->vertexCount);
    for (int i = 0; i < this->vertexCount; i++)
    {
        vertices[i]->printVertex();
    }
    for (int i = 0; i < this->vertexCount; i++)
    {
        printf("vertex: %p\n", this->vertices[i]);
        int vertexFaceCount = this->vertices[i]->getFaceCount();
        printf("vertex face count: %d\n", vertexFaceCount);
        for (int j = 0; j < vertexFaceCount; j++)
        {
            face *temp = this->vertices[i]->getFace(j);
            bool exists = false;
            for (int k = 0; k < faceCount; k++)
            {
                if (faces[k] == temp)
                {
                    exists = true;
                    break;
                }
            }
            if (!exists)
            {
                faceCount++;
                face *tempFaces[faceCount];
                for (int k = 0; k < faceCount - 1; k++)
                {
                    tempFaces[k] = faces[k];
                }
                printf("%p, %d\n", temp, faceCount);
                tempFaces[faceCount - 1] = temp;
                faces = tempFaces;
            }
        }
    }
    *gFaceCount = faceCount;
    return faces;
}

void geo::addVertices(vertex **v, int count)
{
    for (int i = 0; i < count; i++)
    {
        this->addVertex(v[i]);
    }
}