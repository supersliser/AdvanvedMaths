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
    this->rotation = vec(0, 0, 0);
    this->scale = vec(1, 1, 1);
    this->m = m;
    this->vertices = nullptr;
    this->vertexCount = 0;
}

void geo::addVertex(vertex *v)
{
    if (v == NULL)
    {
        return;
    }
    vertex **temp = (vertex **)calloc(this->vertexCount + 1, sizeof(vertex *));
    for (int i = 0; i < vertexCount; i++)
    {
        if (this->vertices[i] == v)
        {
            return;
        }
        temp[i] = this->vertices[i];
    }
    temp[this->vertexCount] = v;
    for (int j = 0; j < vertexCount; j++)
    {
        if (temp[j] == NULL)
        {
            for (int k = j; k < vertexCount; k++)
            {
                temp[k] = temp[k + 1];
            }
            temp = (vertex **)realloc(temp, vertexCount - 1 * sizeof(vertex));
            vertexCount--;
        }
    }
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

hit geo::testRay(point p, vec v, face *face) {
    // Assuming face has 3 vertices
    vertex *v0 = face->getVertices()[0];
    vertex *v1 = face->getVertices()[1];
    vertex *v2 = face->getVertices()[2];

    // Convert vertices to points
    point p0 = v0->getPos();
    point p1 = v1->getPos();
    point p2 = v2->getPos();

    // Calculate normal of the face
    vec edge1 = p1.sub(p0);
    vec edge2 = p2.sub(p0);
    vec normal = edge1.cp(edge2).Normalise();

    // Calculate intersection
    float d = normal.dp(p0.toVec());
    float t = (d - normal.dp(p.toVec())) / normal.dp(v);

    if (t < 0) {
        return hit(false, this, 0, point(), nullptr);
    }

    point intersection = p.add(v.mult(t).toPoint());

    // Check if intersection is inside the triangle
    vec c0 = (p1.sub(p0)).cp(intersection.sub(p0));
    vec c1 = (p2.sub(p1)).cp(intersection.sub(p1));
    vec c2 = (p0.sub(p2)).cp(intersection.sub(p2));

    if (normal.dp(c0) >= 0 && normal.dp(c1) >= 0 && normal.dp(c2) >= 0) {
        return hit(true, this, t, intersection, face);
    }

    return hit(false, this, 0, point(), nullptr);
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

mat *geo::getMat()
{
    return this->m;
}

void geo::setMat(mat *m)
{
    this->m = m;
}

// void geo::makeFace(vertex **vs, int vCount)
// {
//     for (int i = 0; i < vCount; i++)
//     {
//         vs[i]->addConnection(vs[(i - 1) < 0 ? vCount - 1 : (i - 1) % vCount]);
//         vs[i]->addConnection(vs[(i + 1) % vCount]);
//     }

//     face *temp = new face(vs, vCount);

//     for (int i = 0; i < vCount; i++)
//     {
//         vs[i]->addFace(temp);
//     }
// }

face **geo::getFaces(int *gFaceCount)
{
    face **faces = nullptr;
    int faceCount = 0;
    for (int i = 0; i < this->vertexCount; i++)
    {
        int vertexFaceCount = this->vertices[i]->getFaceCount();
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
                face **tempFaces = (face **)calloc(faceCount, sizeof(face));
                for (int k = 0; k < faceCount - 1; k++)
                {
                    tempFaces[k] = faces[k];
                }
                tempFaces[faceCount - 1] = temp;
                faces = tempFaces;
                // temp->printFace();
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

void geo::cube(float size)
{
    // Build a cube
    vertex *v[8];

    // Front (-z)   Back (+z)
    // 3 --- 2      7 --- 6
    // |     |      |     |
    // 0 --- 1      4 --- 5

    v[0] = new vertex(-size, -size, -size); // Front bottom left
    v[1] = new vertex(size, -size, -size);  // Front bottom right
    v[2] = new vertex(size, size, -size);   // Front top right
    v[3] = new vertex(-size, size, -size);  // Front top left
    v[4] = new vertex(-size, -size, size);  // Back bottom left
    v[5] = new vertex(size, -size, size);   // Back bottom right
    v[6] = new vertex(size, size, size);    // Back top right
    v[7] = new vertex(-size, size, size);   // Back top left

    this->addVertices(v, 8);

    // Front face triangles (facing -z)
    this->makeFace(v[0], v[1], v[2]);  // Triangle 1
    this->makeFace(v[0], v[2], v[3]);  // Triangle 2

    // Back face triangles (facing +z)
    this->makeFace(v[5], v[4], v[7]);  // Triangle 1
    this->makeFace(v[5], v[7], v[6]);  // Triangle 2

    // Left face triangles (facing -x)
    this->makeFace(v[4], v[0], v[3]);  // Triangle 1
    this->makeFace(v[4], v[3], v[7]);  // Triangle 2

    // Right face triangles (facing +x)
    this->makeFace(v[1], v[5], v[6]);  // Triangle 1
    this->makeFace(v[1], v[6], v[2]);  // Triangle 2

    // Top face triangles (facing +y)
    this->makeFace(v[3], v[2], v[6]);  // Triangle 1
    this->makeFace(v[3], v[6], v[7]);  // Triangle 2

    // Bottom face triangles (facing -y)
    this->makeFace(v[4], v[5], v[1]);  // Triangle 1
    this->makeFace(v[4], v[1], v[0]);  // Triangle 2
}

void geo::plane(float size)
{
    // Build a plane
    vertex *v[4];

    v[0] = new vertex(-size, 0, -size);
    v[1] = new vertex(size, 0, -size);
    v[2] = new vertex(size, 0, size);
    v[3] = new vertex(-size, 0, size);

    this->addVertices(v, 4);

    // Front face (counter-clockwise order when viewed from positive Z direction)
    // Front face triangles (facing -z)
    this->makeFace(v[0], v[1], v[2]);  // Triangle 1
    this->makeFace(v[0], v[2], v[3]);  // Triangle 2

    int faceCount;
    face **faces = this->getFaces(&faceCount);
    for (int i = 0; i < faceCount; i++)
    {
        faces[i]->printFace();
    }
}

void geo::printGeo()
{
    printf("Geo\n");
}

void geo::makeFace(vertex * v1, vertex * v2, vertex * v3)
{
    v1->addConnection(v2);
    v1->addConnection(v3);
    v2->addConnection(v1);
    v2->addConnection(v3);
    v3->addConnection(v1);
    v3->addConnection(v2);

    face *f = new face(v1, v2, v3);
    v1->addFace(f);
    v2->addFace(f);
    v3->addFace(f);
}