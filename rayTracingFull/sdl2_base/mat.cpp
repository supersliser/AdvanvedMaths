#include "mat.h"
#include <math.h>

mat::mat() {
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

mat::mat(color diffuse, color specular, color ambient, float reflectivity, float roughness) {
    this->diffuse = diffuse;
    this->specular = specular;
    this->ambient = ambient;
    this->reflectivity = reflectivity;
    this->roughness = roughness;
}

color mat::shade(cam c, light l, hit h, sphere *objs, int objCount, int recursionCount, int recursionMax) {
    //diffusion
    color diffuse;

    vec lDir;
    lDir.x = l.pos.x - h.P.x;
    lDir.y = l.pos.y - h.P.y;
    lDir.z = l.pos.z - h.P.z;
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
    diffuse.r = diff * l.col.r * this->diffuse.r;
    diffuse.g = diff * l.col.g * this->diffuse.g;
    diffuse.b = diff * l.col.b * this->diffuse.b;
    // printf("%f, %f, %f\n", diffuse.r, diffuse.g, diffuse.b);

    //specular
    color specular;
    vec H;
    H.x = (c.dir.x + lDir.x) / 2;
    H.y = (c.dir.y + lDir.y) / 2;
    H.z = (c.dir.z + lDir.z) / 2;
    H = H.Normalise();
    float spec = H.dp(h.obj->getNormal(h.P));
    spec = pow(spec, 1/this->roughness);
    if (spec < 0.0f)
    {
        spec = 0.0f;
    }
    if (spec > 1.0f)
    {
        spec = 1.0f;
    }
    specular.r = spec * this->specular.r;
    specular.g = spec * this->specular.g;
    specular.b = spec * this->specular.b;
    // vec rDir;
    // rDir.x = -c.dir.x + 2 * (c.dir.dp(h.obj->getNormal(h.P)) * h.obj->getNormal(h.P).x);
    // rDir.y = -c.dir.y + 2 * (c.dir.dp(h.obj->getNormal(h.P)) * h.obj->getNormal(h.P).y);
    // rDir.z = -c.dir.z + 2 * (c.dir.dp(h.obj->getNormal(h.P)) * h.obj->getNormal(h.P).z);
    // rDir = rDir.Normalise();
    // float spec = rDir.dp(lDir);
    // if (spec < 0.0f)
    // {
    //     spec = 0.0f;
    // }
    // if (spec > 1.0f)
    // {
    //     spec = 1.0f;
    // }
    // specular.r = spec * l.col.r * this->specular.r;
    // specular.g = spec * l.col.g * this->specular.g;
    // specular.b = spec * l.col.b * this->specular.b;
    // printf("%f, %f, %f\n", specular.r, specular.g, specular.b);

    //ambient
    color ambient;
    ambient.r = l.col.r * this->ambient.r;
    ambient.g = l.col.g * this->ambient.g;
    ambient.b = l.col.b * this->ambient.b;
    // printf("%f, %f, %f\n", ambient.r, ambient.g, ambient.b);

    //reflection
    color reflection;
    reflection.r = reflectivity * reflect(*h.obj, h.P, h.obj->getNormal(h.P), c.dir, objs, objCount, l, recursionCount, recursionMax).r;
    reflection.g = reflectivity * reflect(*h.obj, h.P, h.obj->getNormal(h.P), c.dir, objs, objCount, l, recursionCount, recursionMax).g;
    reflection.b = reflectivity * reflect(*h.obj, h.P, h.obj->getNormal(h.P), c.dir, objs, objCount, l, recursionCount, recursionMax).b;
    // printf("%f, %f, %f\n", reflection.r, reflection.g, reflection.b);

    //shadow
    bool shadow = l.calculateShadows(h, objs, objCount);
    // printf("%d\n", shadow);
    //add
    color output;
    diffuse.normaliseF();
    specular.normaliseF();
    ambient.normaliseF();
    reflection.normaliseF();
    output.r = ((diffuse.r + specular.r + ambient.r + reflection.r) / 4) * (shadow ? 0.0 : 1.0);
    output.g = ((diffuse.g + specular.g + ambient.g + reflection.g) / 4) * (shadow ? 0.0 : 1.0);
    output.b = ((diffuse.b + specular.b + ambient.b + reflection.b) / 4) * (shadow ? 0.0 : 1.0);
    // printf("%f, %f, %f\n", output.r, output.g, output.b);
    return output;
}

color mat::reflect(sphere current, point p, vec normal, vec i, sphere *objs, int objCount, light l, int recursionCount, int recursionMax) {
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
            return h.obj->m->shade(c, l, h, objs, objCount, recursionCount + 1, recursionMax);
        }
    }
    // printf("failed\n");
	color o;
	o.r = 0;
	o.g = 0;
	o.b = 0;
	return o;
}

void mat::logMat() {
    printf("diffuse: %f, %f, %f\n", this->diffuse.r, this->diffuse.g, this->diffuse.b);
    printf("specular: %f, %f, %f\n", this->specular.r, this->specular.g, this->specular.b);
    printf("ambient: %f, %f, %f\n", this->ambient.r, this->ambient.g, this->ambient.b);
    printf("reflectivity: %f\n", this->reflectivity);
    printf("roughness: %f\n", this->roughness);
}