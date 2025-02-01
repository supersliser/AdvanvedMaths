#include "geo.h"
#include <cmath>

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
    free(this->vertices);
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

hit geo::testRay(point p, vec v, face *face)
{
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

    if (t < 0)
    {
        return hit(false, this, 0, point(), nullptr);
    }

    point intersection = p.add(v.mult(t).toPoint());

    // Check if intersection is inside the triangle
    vec c0 = (p1.sub(p0)).cp(intersection.sub(p0));
    vec c1 = (p2.sub(p1)).cp(intersection.sub(p1));
    vec c2 = (p0.sub(p2)).cp(intersection.sub(p2));

    if (normal.dp(c0) >= 0 && normal.dp(c1) >= 0 && normal.dp(c2) >= 0)
    {
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
                free(faces);
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
    this->makeFace(v[0], v[1], v[2]); // Triangle 1
    this->makeFace(v[0], v[2], v[3]); // Triangle 2

    // Back face triangles (facing +z)
    this->makeFace(v[5], v[4], v[7]); // Triangle 1
    this->makeFace(v[5], v[7], v[6]); // Triangle 2

    // Left face triangles (facing -x)
    this->makeFace(v[4], v[0], v[3]); // Triangle 1
    this->makeFace(v[4], v[3], v[7]); // Triangle 2

    // Right face triangles (facing +x)
    this->makeFace(v[1], v[5], v[6]); // Triangle 1
    this->makeFace(v[1], v[6], v[2]); // Triangle 2

    // Top face triangles (facing +y)
    this->makeFace(v[3], v[2], v[6]); // Triangle 1
    this->makeFace(v[3], v[6], v[7]); // Triangle 2

    // Bottom face triangles (facing -y)
    this->makeFace(v[4], v[5], v[1]); // Triangle 1
    this->makeFace(v[4], v[1], v[0]); // Triangle 2
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
    this->makeFace(v[0], v[1], v[2]); // Triangle 1
    this->makeFace(v[0], v[2], v[3]); // Triangle 2

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

void geo::makeFace(vertex *v1, vertex *v2, vertex *v3)
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

void geo::teddyBear(float size)
{

    // Build a low poly teddy bear
    // Body
    vertex *body[8];
    float bodySize = size * 0.5;
    body[0] = new vertex(-bodySize, -bodySize, -bodySize);
    body[1] = new vertex(bodySize, -bodySize, -bodySize);
    body[2] = new vertex(bodySize, bodySize, -bodySize);
    body[3] = new vertex(-bodySize, bodySize, -bodySize);
    body[4] = new vertex(-bodySize, -bodySize, bodySize);
    body[5] = new vertex(bodySize, -bodySize, bodySize);
    body[6] = new vertex(bodySize, bodySize, bodySize);
    body[7] = new vertex(-bodySize, bodySize, bodySize);
    this->addVertices(body, 8);

    // Body faces
    this->makeFace(body[0], body[1], body[2]);
    this->makeFace(body[0], body[2], body[3]);
    this->makeFace(body[4], body[5], body[6]);
    this->makeFace(body[4], body[6], body[7]);
    this->makeFace(body[0], body[1], body[5]);
    this->makeFace(body[0], body[5], body[4]);
    this->makeFace(body[1], body[2], body[6]);
    this->makeFace(body[1], body[6], body[5]);
    this->makeFace(body[2], body[3], body[7]);
    this->makeFace(body[2], body[7], body[6]);
    this->makeFace(body[3], body[0], body[4]);
    this->makeFace(body[3], body[4], body[7]);

    // Head
    vertex *head[8];
    float headSize = size * 0.3;
    head[0] = new vertex(-headSize, bodySize, -headSize);
    head[1] = new vertex(headSize, bodySize, -headSize);
    head[2] = new vertex(headSize, bodySize + headSize * 2, -headSize);
    head[3] = new vertex(-headSize, bodySize + headSize * 2, -headSize);
    head[4] = new vertex(-headSize, bodySize, headSize);
    head[5] = new vertex(headSize, bodySize, headSize);
    head[6] = new vertex(headSize, bodySize + headSize * 2, headSize);
    head[7] = new vertex(-headSize, bodySize + headSize * 2, headSize);
    this->addVertices(head, 8);

    // Head faces
    this->makeFace(head[0], head[1], head[2]);
    this->makeFace(head[0], head[2], head[3]);
    this->makeFace(head[4], head[5], head[6]);
    this->makeFace(head[4], head[6], head[7]);
    this->makeFace(head[0], head[1], head[5]);
    this->makeFace(head[0], head[5], head[4]);
    this->makeFace(head[1], head[2], head[6]);
    this->makeFace(head[1], head[6], head[5]);
    this->makeFace(head[2], head[3], head[7]);
    this->makeFace(head[2], head[7], head[6]);
    this->makeFace(head[3], head[0], head[4]);
    this->makeFace(head[3], head[4], head[7]);

    // Ears
    vertex *ear1[8];
    float earSize = size * 0.15;
    ear1[0] = new vertex(-earSize, bodySize + headSize, -earSize);
    ear1[1] = new vertex(earSize, bodySize + headSize, -earSize);
    ear1[2] = new vertex(earSize, bodySize + headSize + earSize * 2, -earSize);
    ear1[3] = new vertex(-earSize, bodySize + headSize + earSize * 2, -earSize);
    ear1[4] = new vertex(-earSize, bodySize + headSize, earSize);
    ear1[5] = new vertex(earSize, bodySize + headSize, earSize);
    ear1[6] = new vertex(earSize, bodySize + headSize + earSize * 2, earSize);
    ear1[7] = new vertex(-earSize, bodySize + headSize + earSize * 2, earSize);
    this->addVertices(ear1, 8);

    // Ear1 faces
    this->makeFace(ear1[0], ear1[1], ear1[2]);
    this->makeFace(ear1[0], ear1[2], ear1[3]);
    this->makeFace(ear1[4], ear1[5], ear1[6]);
    this->makeFace(ear1[4], ear1[6], ear1[7]);
    this->makeFace(ear1[0], ear1[1], ear1[5]);
    this->makeFace(ear1[0], ear1[5], ear1[4]);
    this->makeFace(ear1[1], ear1[2], ear1[6]);
    this->makeFace(ear1[1], ear1[6], ear1[5]);
    this->makeFace(ear1[2], ear1[3], ear1[7]);
    this->makeFace(ear1[2], ear1[7], ear1[6]);
    this->makeFace(ear1[3], ear1[0], ear1[4]);
    this->makeFace(ear1[3], ear1[4], ear1[7]);

    vertex *ear2[8];
    ear2[0] = new vertex(-earSize, bodySize + headSize, -earSize);
    ear2[1] = new vertex(earSize, bodySize + headSize, -earSize);
    ear2[2] = new vertex(earSize, bodySize + headSize + earSize * 2, -earSize);
    ear2[3] = new vertex(-earSize, bodySize + headSize + earSize * 2, -earSize);
    ear2[4] = new vertex(-earSize, bodySize + headSize, earSize);
    ear2[5] = new vertex(earSize, bodySize + headSize, earSize);
    ear2[6] = new vertex(earSize, bodySize + headSize + earSize * 2, earSize);
    ear2[7] = new vertex(-earSize, bodySize + headSize + earSize * 2, earSize);
    this->addVertices(ear2, 8);

    // Ear2 faces
    this->makeFace(ear2[0], ear2[1], ear2[2]);
    this->makeFace(ear2[0], ear2[2], ear2[3]);
    this->makeFace(ear2[4], ear2[5], ear2[6]);
    this->makeFace(ear2[4], ear2[6], ear2[7]);
    this->makeFace(ear2[0], ear2[1], ear2[5]);
    this->makeFace(ear2[0], ear2[5], ear2[4]);
    this->makeFace(ear2[1], ear2[2], ear2[6]);
    this->makeFace(ear2[1], ear2[6], ear2[5]);
    this->makeFace(ear2[2], ear2[3], ear2[7]);
    this->makeFace(ear2[2], ear2[7], ear2[6]);
    this->makeFace(ear2[3], ear2[0], ear2[4]);
    this->makeFace(ear2[3], ear2[4], ear2[7]);

    // Arms
    vertex *arm1[8];
    float armSize = size * 0.2;
    arm1[0] = new vertex(-armSize, -armSize, -armSize);
    arm1[1] = new vertex(armSize, -armSize, -armSize);
    arm1[2] = new vertex(armSize, armSize * 2, -armSize);
    arm1[3] = new vertex(-armSize, armSize * 2, -armSize);
    arm1[4] = new vertex(-armSize, -armSize, armSize);
    arm1[5] = new vertex(armSize, -armSize, armSize);
    arm1[6] = new vertex(armSize, armSize * 2, armSize);
    arm1[7] = new vertex(-armSize, armSize * 2, armSize);
    this->addVertices(arm1, 8);

    // Arm1 faces
    this->makeFace(arm1[0], arm1[1], arm1[2]);
    this->makeFace(arm1[0], arm1[2], arm1[3]);
    this->makeFace(arm1[4], arm1[5], arm1[6]);
    this->makeFace(arm1[4], arm1[6], arm1[7]);
    this->makeFace(arm1[0], arm1[1], arm1[5]);
    this->makeFace(arm1[0], arm1[5], arm1[4]);
    this->makeFace(arm1[1], arm1[2], arm1[6]);
    this->makeFace(arm1[1], arm1[6], arm1[5]);
    this->makeFace(arm1[2], arm1[3], arm1[7]);
    this->makeFace(arm1[2], arm1[7], arm1[6]);
    this->makeFace(arm1[3], arm1[0], arm1[4]);
    this->makeFace(arm1[3], arm1[4], arm1[7]);

    vertex *arm2[8];
    arm2[0] = new vertex(-armSize, -armSize, -armSize);
    arm2[1] = new vertex(armSize, -armSize, -armSize);
    arm2[2] = new vertex(armSize, armSize * 2, -armSize);
    arm2[3] = new vertex(-armSize, armSize * 2, -armSize);
    arm2[4] = new vertex(-armSize, -armSize, armSize);
    arm2[5] = new vertex(armSize, -armSize, armSize);
    arm2[6] = new vertex(armSize, armSize * 2, armSize);
    arm2[7] = new vertex(-armSize, armSize * 2, armSize);
    this->addVertices(arm2, 8);

    // Arm2 faces
    this->makeFace(arm2[0], arm2[1], arm2[2]);
    this->makeFace(arm2[0], arm2[2], arm2[3]);
    this->makeFace(arm2[4], arm2[5], arm2[6]);
    this->makeFace(arm2[4], arm2[6], arm2[7]);
    this->makeFace(arm2[0], arm2[1], arm2[5]);
    this->makeFace(arm2[0], arm2[5], arm2[4]);
    this->makeFace(arm2[1], arm2[2], arm2[6]);
    this->makeFace(arm2[1], arm2[6], arm2[5]);
    this->makeFace(arm2[2], arm2[3], arm2[7]);
    this->makeFace(arm2[2], arm2[7], arm2[6]);
    this->makeFace(arm2[3], arm2[0], arm2[4]);
    this->makeFace(arm2[3], arm2[4], arm2[7]);

    // Legs
    vertex *leg1[8];
    float legSize = size * 0.25;
    leg1[0] = new vertex(-legSize, -legSize, -legSize);
    leg1[1] = new vertex(legSize, -legSize, -legSize);
    leg1[2] = new vertex(legSize, legSize * 2, -legSize);
    leg1[3] = new vertex(-legSize, legSize * 2, -legSize);
    leg1[4] = new vertex(-legSize, -legSize, legSize);
    leg1[5] = new vertex(legSize, -legSize, legSize);
    leg1[6] = new vertex(legSize, legSize * 2, legSize);
    leg1[7] = new vertex(-legSize, legSize * 2, legSize);
    this->addVertices(leg1, 8);

    // Leg1 faces
    this->makeFace(leg1[0], leg1[1], leg1[2]);
    this->makeFace(leg1[0], leg1[2], leg1[3]);
    this->makeFace(leg1[4], leg1[5], leg1[6]);
    this->makeFace(leg1[4], leg1[6], leg1[7]);
    this->makeFace(leg1[0], leg1[1], leg1[5]);
    this->makeFace(leg1[0], leg1[5], leg1[4]);
    this->makeFace(leg1[1], leg1[2], leg1[6]);
    this->makeFace(leg1[1], leg1[6], leg1[5]);
    this->makeFace(leg1[2], leg1[3], leg1[7]);
    this->makeFace(leg1[2], leg1[7], leg1[6]);
    this->makeFace(leg1[3], leg1[0], leg1[4]);
    this->makeFace(leg1[3], leg1[4], leg1[7]);

    vertex *leg2[8];
    leg2[0] = new vertex(-legSize, -legSize, -legSize);
    leg2[1] = new vertex(legSize, -legSize, -legSize);
    leg2[2] = new vertex(legSize, legSize * 2, -legSize);
    leg2[3] = new vertex(-legSize, legSize * 2, -legSize);
    leg2[4] = new vertex(-legSize, -legSize, legSize);
    leg2[5] = new vertex(legSize, -legSize, legSize);
    leg2[6] = new vertex(legSize, legSize * 2, legSize);
    leg2[7] = new vertex(-legSize, legSize * 2, legSize);
    this->addVertices(leg2, 8);

    // Leg2 faces
    this->makeFace(leg2[0], leg2[1], leg2[2]);
    this->makeFace(leg2[0], leg2[2], leg2[3]);
    this->makeFace(leg2[4], leg2[5], leg2[6]);
    this->makeFace(leg2[4], leg2[6], leg2[7]);
    this->makeFace(leg2[0], leg2[1], leg2[5]);
    this->makeFace(leg2[0], leg2[5], leg2[4]);
    this->makeFace(leg2[1], leg2[2], leg2[6]);
    this->makeFace(leg2[1], leg2[6], leg2[5]);
    this->makeFace(leg2[2], leg2[3], leg2[7]);
    this->makeFace(leg2[2], leg2[7], leg2[6]);
    this->makeFace(leg2[3], leg2[0], leg2[4]);
    this->makeFace(leg2[3], leg2[4], leg2[7]);
}

void geo::pill(float size)
{
    // Build a pill shape (capsule)
    vertex *v[16];

    // Bottom hemisphere
    v[0] = new vertex(-size, -size, -size);
    v[1] = new vertex(size, -size, -size);
    v[2] = new vertex(size, size, -size);
    v[3] = new vertex(-size, size, -size);
    v[4] = new vertex(-size, -size, size);
    v[5] = new vertex(size, -size, size);
    v[6] = new vertex(size, size, size);
    v[7] = new vertex(-size, size, size);

    // Top hemisphere
    v[8] = new vertex(-size, -size, size * 2);
    v[9] = new vertex(size, -size, size * 2);
    v[10] = new vertex(size, size, size * 2);
    v[11] = new vertex(-size, size, size * 2);
    v[12] = new vertex(-size, -size, size * 3);
    v[13] = new vertex(size, -size, size * 3);
    v[14] = new vertex(size, size, size * 3);
    v[15] = new vertex(-size, size, size * 3);

    this->addVertices(v, 16);

    // Bottom hemisphere faces
    this->makeFace(v[0], v[1], v[2]);
    this->makeFace(v[0], v[2], v[3]);
    this->makeFace(v[4], v[5], v[6]);
    this->makeFace(v[4], v[6], v[7]);
    this->makeFace(v[0], v[1], v[5]);
    this->makeFace(v[0], v[5], v[4]);
    this->makeFace(v[1], v[2], v[6]);
    this->makeFace(v[1], v[6], v[5]);
    this->makeFace(v[2], v[3], v[7]);
    this->makeFace(v[2], v[7], v[6]);
    this->makeFace(v[3], v[0], v[4]);
    this->makeFace(v[3], v[4], v[7]);

    // Top hemisphere faces
    this->makeFace(v[8], v[9], v[10]);
    this->makeFace(v[8], v[10], v[11]);
    this->makeFace(v[12], v[13], v[14]);
    this->makeFace(v[12], v[14], v[15]);
    this->makeFace(v[8], v[9], v[13]);
    this->makeFace(v[8], v[13], v[12]);
    this->makeFace(v[9], v[10], v[14]);
    this->makeFace(v[9], v[14], v[13]);
    this->makeFace(v[10], v[11], v[15]);
    this->makeFace(v[10], v[15], v[14]);
    this->makeFace(v[11], v[8], v[12]);
    this->makeFace(v[11], v[12], v[15]);

    // Connecting faces
    this->makeFace(v[4], v[5], v[9]);
    this->makeFace(v[4], v[9], v[8]);
    this->makeFace(v[5], v[6], v[10]);
    this->makeFace(v[5], v[10], v[9]);
    this->makeFace(v[6], v[7], v[11]);
    this->makeFace(v[6], v[11], v[10]);
    this->makeFace(v[7], v[4], v[8]);
    this->makeFace(v[7], v[8], v[11]);
}

void geo::sphere(float size, int stacks, int slices)
{
    vertex **v = new vertex *[stacks * slices];

    for (int i = 0; i < stacks; ++i)
    {
        float phi = M_PI * i / (stacks - 1);
        for (int j = 0; j < slices; ++j)
        {
            float theta = 2 * M_PI * j / (slices - 1);
            float x = size * sin(phi) * cos(theta);
            float y = size * sin(phi) * sin(theta);
            float z = size * cos(phi);
            v[i * slices + j] = new vertex(x, y, z);
        }
    }

    this->addVertices(v, stacks * slices);

    for (int i = 0; i < stacks - 1; ++i)
    {
        for (int j = 0; j < slices - 1; ++j)
        {
            this->makeFace(v[i * slices + j], v[i * slices + (j + 1)], v[(i + 1) * slices + j]);
            this->makeFace(v[i * slices + (j + 1)], v[(i + 1) * slices + (j + 1)], v[(i + 1) * slices + j]);
        }
    }

    delete[] v;
}

void geo::cylinder(float size)
{
    const int stacks = 10;
    const int slices = 10;
    vertex **v = new vertex *[stacks * slices];

    for (int i = 0; i < stacks; ++i)
    {
        float phi = size * i / (stacks - 1);
        for (int j = 0; j < slices; ++j)
        {
            float theta = 2 * M_PI * j / (slices - 1);
            float x = sin(theta);
            float y = cos(theta);
            float z = phi;
            v[i * slices + j] = new vertex(x, y, z);
        }
    }

    this->addVertices(v, stacks * slices);

    for (int i = 0; i < stacks - 1; ++i)
    {
        for (int j = 0; j < slices - 1; ++j)
        {
            this->makeFace(v[i * slices + j], v[i * slices + (j + 1)], v[(i + 1) * slices + j]);
            this->makeFace(v[i * slices + (j + 1)], v[(i + 1) * slices + (j + 1)], v[(i + 1) * slices + j]);
        }
    }

    delete[] v;
}

void geo::cone(float size)
{
    const int stacks = 10;
    const int slices = 10;
    vertex **v = new vertex *[stacks * slices];

    for (int i = 0; i < stacks; ++i)
    {
        float phi = size * i / (stacks - 1);
        for (int j = 0; j < slices; ++j)
        {
            float theta = 2 * M_PI * j / (slices - 1);
            float x = phi * sin(theta);
            float y = phi * cos(theta);
            float z = size - phi;
            v[i * slices + j] = new vertex(x, y, z);
        }
    }

    this->addVertices(v, stacks * slices);

    for (int i = 0; i < stacks - 1; ++i)
    {
        for (int j = 0; j < slices - 1; ++j)
        {
            this->makeFace(v[i * slices + j], v[i * slices + (j + 1)], v[(i + 1) * slices + j]);
            this->makeFace(v[i * slices + (j + 1)], v[(i + 1) * slices + (j + 1)], v[(i + 1) * slices + j]);
        }
    }

    delete[] v;
}

void geo::torus(float size)
{
    const int stacks = 10;
    const int slices = 10;
    vertex **v = new vertex *[stacks * slices];

    for (int i = 0; i < stacks; ++i)
    {
        float phi = 2 * M_PI * i / (stacks - 1);
        for (int j = 0; j < slices; ++j)
        {
            float theta = 2 * M_PI * j / (slices - 1);
            float x = (1 + 0.5 * cos(theta)) * cos(phi);
            float y = (1 + 0.5 * cos(theta)) * sin(phi);
            float z = 0.5 * sin(theta);
            v[i * slices + j] = new vertex(x, y, z);
        }
    }

    this->addVertices(v, stacks * slices);

    for (int i = 0; i < stacks - 1; ++i)
    {
        for (int j = 0; j < slices - 1; ++j)
        {
            this->makeFace(v[i * slices + j], v[i * slices + (j + 1)], v[(i + 1) * slices + j]);
            this->makeFace(v[i * slices + (j + 1)], v[(i + 1) * slices + (j + 1)], v[(i + 1) * slices + j]);
        }
    }

    delete[] v;
}