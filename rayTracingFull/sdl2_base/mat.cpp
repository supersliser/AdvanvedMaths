#include "mat.h"
#include <math.h>

mat::mat()
{
    diffuse.r = 0;
    diffuse.g = 0;
    diffuse.b = 0;
    specular.r = 0;
    specular.g = 0;
    specular.b = 0;
    ambient.r = 0;
    ambient.g = 0;
    ambient.b = 0;
    reflectivity = 0;
    roughness = 0;
}

mat::mat(color diffuse, color specular, color ambient, float reflectivity, float roughness)
{
    this->diffuse = diffuse;
    this->specular = specular;
    this->ambient = ambient;
    this->reflectivity = reflectivity;
    this->roughness = roughness;
}

color mat::shade(cam c, light *ls, int lCount, hit h, sphere *objs, int objCount, int recursionCount, int recursionMax)
{
    // diffusion
    color finalDiffuse;

    for (int i = 0; i < lCount; i++)
    {
        vec lDir;
        lDir.x = ls[i].pos.x - h.P.x;
        lDir.y = ls[i].pos.y - h.P.y;
        lDir.z = ls[i].pos.z - h.P.z;
        lDir = lDir.Normalise();
        float diff = lDir.dp(h.obj->getNormal(h.P));
        if (diff < 0.0f)
        {
            diff = 0.0f;
        }
        if (diff > 1.0f)
        {
            diff = 1.0f;
        }
        finalDiffuse.r += diff * ls[i].col.r * this->diffuse.r;
        finalDiffuse.g += diff * ls[i].col.g * this->diffuse.g;
        finalDiffuse.b += diff * ls[i].col.b * this->diffuse.b;
        // printf("%f, %f, %f\n", diffuse.r, diffuse.g, diffuse.b);
    }
    finalDiffuse.r = finalDiffuse.r / lCount;
    finalDiffuse.g = finalDiffuse.g / lCount;
    finalDiffuse.b = finalDiffuse.b / lCount;

    // printf("diffuse: %f, %f, %f\n", finalDiffuse.r, finalDiffuse.g, finalDiffuse.b);

    // specular
    color finalSpecular;

    for (int i = 0; i < lCount; i++)
    {
        vec H;
        vec lDir;
        lDir.x = ls[i].pos.x - h.P.x;
        lDir.y = ls[i].pos.y - h.P.y;
        lDir.z = ls[i].pos.z - h.P.z;
        H.x = (c.dir.x + lDir.x) / 2;
        H.y = (c.dir.y + lDir.y) / 2;
        H.z = (c.dir.z + lDir.z) / 2;
        H = H.Normalise();
        float spec = H.dp(h.obj->getNormal(h.P));
        spec = pow(spec, 1 / this->roughness);
        if (spec < 0.0f)
        {
            spec = 0.0f;
        }
        if (spec > 1.0f)
        {
            spec = 1.0f;
        }
        finalSpecular.r += spec * this->specular.r;
        finalSpecular.g += spec * this->specular.g;
        finalSpecular.b += spec * this->specular.b;
    }
    finalSpecular.r = finalSpecular.r / lCount;
    finalSpecular.g = finalSpecular.g / lCount;
    finalSpecular.b = finalSpecular.b / lCount;

    // printf("%f, %f, %f\n", finalSpecular.r, finalSpecular.g, finalSpecular.b);

    // ambient
    color finalAmbient;
    for (int i = 0; i < lCount; i++)
    {
        finalAmbient.r += ls[i].col.r * this->ambient.r;
        finalAmbient.g += ls[i].col.g * this->ambient.g;
        finalAmbient.b += ls[i].col.b * this->ambient.b;
    }
    finalAmbient.r = finalAmbient.r / lCount;
    finalAmbient.g = finalAmbient.g / lCount;
    finalAmbient.b = finalAmbient.b / lCount;

    // printf("%f, %f, %f\n", finalAmbient.r, finalAmbient.g, finalAmbient.b);

    // reflection
    color reflection;
    reflection.r = reflectivity * reflect(*h.obj, h.P, h.obj->getNormal(h.P), c.dir, objs, objCount, ls, lCount, recursionCount, recursionMax).r;
    reflection.g = reflectivity * reflect(*h.obj, h.P, h.obj->getNormal(h.P), c.dir, objs, objCount, ls, lCount, recursionCount, recursionMax).g;
    reflection.b = reflectivity * reflect(*h.obj, h.P, h.obj->getNormal(h.P), c.dir, objs, objCount, ls, lCount, recursionCount, recursionMax).b;
    // printf("%f, %f, %f\n", reflection.r, reflection.g, reflection.b);

    // shadow
    bool shadows[lCount];
    for (int i = 0; i < lCount; i++)
    {
        shadows[i] = ls[i].calculateShadows(h, objs, objCount);
    }
    float shadow = lCount;
    for (int i = 0; i < lCount; i++)
    {
        if (shadows[i])
        {
            shadow--;
        }
    }
    shadow /= lCount;
    // printf("%f\n", shadow);
    // add
    color output;
    finalDiffuse.normaliseF();
    finalSpecular.normaliseF();
    finalAmbient.normaliseF();
    reflection.normaliseF();
    output.r = ((finalDiffuse.r + finalSpecular.r + finalAmbient.r + reflection.r / 4) * shadow);
    output.g = ((finalDiffuse.g + finalSpecular.g + finalAmbient.g + reflection.g / 4) * shadow);
    output.b = ((finalDiffuse.b + finalSpecular.b + finalAmbient.b + reflection.b / 4) * shadow);
    // printf("%f, %f, %f\n", output.r, output.g, output.b);
    return output;
}

color mat::reflect(sphere current, point p, vec normal, vec i, sphere *objs, int objCount, light *ls, int lCount, int recursionCount, int recursionMax)
{
    vec r;
    r.x = i.x - 2 * (i.dp(normal) * normal.x);
    r.y = i.y - 2 * (i.dp(normal) * normal.y);
    r.z = i.z - 2 * (i.dp(normal) * normal.z);

    hit h = cam().TraceObjs(cam(p, r, 1, false), objs, objCount);

    if (h.hitSuccess)
    {
        cam c;
        c.pos = p;
        c.dir = r;
        // printf("hit\n");
        if (recursionCount < recursionMax)
        {
            // printf("recursion\n");
            color temp = h.obj->m->shade(c, ls, lCount, h, objs, objCount, recursionCount + 1, recursionMax);
            return temp;
        }
    }
    // printf("failed\n");
    color o;
    o.r = 0;
    o.g = 0;
    o.b = 0;
    return o;
}

void mat::logMat()
{
    printf("diffuse: %f, %f, %f\n", this->diffuse.r, this->diffuse.g, this->diffuse.b);
    printf("specular: %f, %f, %f\n", this->specular.r, this->specular.g, this->specular.b);
    printf("ambient: %f, %f, %f\n", this->ambient.r, this->ambient.g, this->ambient.b);
    printf("reflectivity: %f\n", this->reflectivity);
    printf("roughness: %f\n", this->roughness);
}