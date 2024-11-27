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
    diffuse.r = h.obj->getNormal(h.P).x * l.col.r * diffuse.r;
    diffuse.g = h.obj->getNormal(h.P).y * l.col.g * diffuse.g;
    diffuse.b = h.obj->getNormal(h.P).z * l.col.b * diffuse.b;
    // printf("%f, %f, %f\n", diffuse.r, diffuse.g, diffuse.b);
    //specular
    color specular;
    vec lDir;
    lDir.x = l.pos.x - h.P.x;
    lDir.y = l.pos.y - h.P.y;
    lDir.z = l.pos.z - h.P.z;
    lDir = lDir.Normalise();
    vec v;
    v.x = c.pos.x - h.P.x;
    v.y = c.pos.y - h.P.y;
    v.z = c.pos.z - h.P.z;
    v = v.Normalise();
    float spec = v.dp(h.obj->getNormal(h.P));
    spec = pow(spec, roughness);
    if (spec < 0.0f)
    {
        spec = 0.0f;
    }
    if (spec > 1.0f)
    {
        spec = 1.0f;
    }
    specular.r = pow(spec, roughness) * l.col.r * specular.r;
    specular.g = pow(spec, roughness) * l.col.g * specular.g;
    specular.b = pow(spec, roughness) * l.col.b * specular.b;
    //ambient
    color ambient;
    ambient.r = l.col.r * ambient.r;
    ambient.g = l.col.g * ambient.g;
    ambient.b = l.col.b * ambient.b;
    // printf("%f, %f, %f\n", specular.r, specular.g, specular.b);
    //reflection
    color reflection;
    reflection.r = reflectivity * reflect(*h.obj, h.P, h.obj->getNormal(h.P), c.dir, objs, objCount, l, recursionCount, recursionMax).r;
    reflection.g = reflectivity * reflect(*h.obj, h.P, h.obj->getNormal(h.P), c.dir, objs, objCount, l, recursionCount, recursionMax).g;
    reflection.b = reflectivity * reflect(*h.obj, h.P, h.obj->getNormal(h.P), c.dir, objs, objCount, l, recursionCount, recursionMax).b;
    //shadow
    bool shadow = l.calculateShadows(h, objs, objCount);
    //add
    color output;
    output.r = ((diffuse.r + specular.r + ambient.r + reflection.r) / 4) * !shadow;
    output.g = ((diffuse.g + specular.g + ambient.g + reflection.g) / 4) * !shadow;
    output.b = ((diffuse.b + specular.b + ambient.b + reflection.b) / 4) * !shadow;
    return output;
}

color mat::reflect(sphere current, point p, vec normal, vec i, sphere *objs, int objCount, light l, int recursionCount, int recursionMax) {
    vec r;
    r.x = i.x - 2 * (i.dp(normal) * normal.x);
	r.y = i.y - 2 * (i.dp(normal) * normal.y);
	r.z = i.z - 2 * (i.dp(normal) * normal.z);

    hit *bestHit = new hit();
    bestHit->P.z = 10000000000;
	for (int i = 0; i < objCount; i++)
	{
		hit h = objs[i].traceObj(p, r);
		if (h.hitSuccess && h.obj->id != current.id)
		{
            bestHit = &h;
			// printf("object\n");
		}
	}
    if (bestHit->hitSuccess)
    {
        cam c;
        c.pos = p;
        c.dir = r;
        if (recursionCount < recursionMax)
        {
            return bestHit->obj->m->shade(c, l, *bestHit, objs, objCount, recursionCount + 1, recursionMax);
        }
        else
        {
            return bestHit->obj->m->shade(c, l, *bestHit, objs, objCount, recursionCount, recursionMax);
        }
    }
	color o;
	o.r = -1;
	o.g = -1;
	o.b = -1;
	return o;
}