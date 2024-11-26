#include "mat.h"

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

color mat::shade(cam c, light l, hit h, sphere *objs, int objCount, int objCurrent, int recursionCount, int recursionMax) {
    //diffusion
    color diffuse;
    diffuse.r = h.normal.x * l.col.r * h.m.diffuse.r;
    diffuse.g = h.normal.y * l.col.g * h.m.diffuse.g;
    diffuse.b = h.normal.z * l.col.b * h.m.diffuse.b;
    // printf("%f, %f, %f\n", diffuse.r, diffuse.g, diffuse.b);
    //specular
    color specular;
    vec lDir;
    lDir.x = l.pos.x - h.P.x;
    lDir.y = l.pos.y - h.P.y;
    lDir.z = l.pos.z - h.P.z;
    lDir = Normalise(lDir);
    vec v;
    v.x = c.pos.x - h.P.x;
    v.y = c.pos.y - h.P.y;
    v.z = c.pos.z - h.P.z;
    v = Normalise(v);
    float diff = dp(lDir, h.normal);
    float spec = dp(v, Normalise(lDir));
    specular.r = pow(spec, h.m.roughness) * l.col.r * h.m.specular.r;
    specular.g = pow(spec, h.m.roughness) * l.col.g * h.m.specular.g;
    specular.b = pow(spec, h.m.roughness) * l.col.b * h.m.specular.b;
    //ambient
    color ambient;
    ambient.r = l.col.r * h.m.ambient.r;
    ambient.g = l.col.g * h.m.ambient.g;
    ambient.b = l.col.b * h.m.ambient.b;
    // printf("%f, %f, %f\n", specular.r, specular.g, specular.b);
    //reflection
    color reflection;
    reflection.r = h.m.reflectivity * reflect(objs[objCurrent], h.P, h.normal, c.dir, objs, objCount, l, recursionCount, recursionMax, h.m.reflectivity, h.m.roughness);
    reflection.g = h.m.reflectivity * reflect(objs[objCurrent], h.P, h.normal, c.dir, objs, objCount, l, recursionCount, recursionMax, h.m.reflectivity, h.m.roughness);
    reflection.b = h.m.reflectivity * reflect(objs[objCurrent], h.P, h.normal, c.dir, objs, objCount, l, recursionCount, recursionMax, h.m.reflectivity, h.m.roughness);
    //shadow
    bool shadow = l.shadow(h, objs, objCount);
    //add
    color output;
    output.r = ((diffuse.r + specular.r + ambient.r + reflection.r + shadow.r) / 5) * !shadow;
    output.g = ((diffuse.g + specular.g + ambient.g + reflection.g + shadow.g) / 5) * !shadow;
    output.b = ((diffuse.b + specular.b + ambient.b + reflection.b + shadow.b) / 5) * !shadow;
    return output;
}

color mat::reflect(sphere current, point p, vec normal, vec i, sphere *objs, int objCount, light l, int recursionCount, int recursionMax) {
    vec r;
    r.x = i.x - 2 * (dp(i, normal) * normal.x);
	r.y = i.y - 2 * (dp(i, normal) * normal.y);
	r.z = i.z - 2 * (dp(i, normal) * normal.z);

	for (int i = objCount; i > 0; i--)
	{
		hit h = traceObj(p, r, objs[i]);
		if (h.hit && h.obj.id != current.id)
		{
			// printf("object\n");
			cam newC;
			newC.dir = r;
			newC.pos = p;
			bool shad = shadow(newC, l, h, objs, objCount);
			color s;
			if (shad)
			{
				s.r = 0;
				s.g = 0;
				s.b = 0;
			}
			else
			{
				recursionCount++;
				s = shade(newC, l, h, roughness, reflectivity, objs, objCount, recursionCount, recursionMax);
			}
            vec temp = s.toVec();
            temp.mult(0.25);
            s = temp.toCol();
			return s;
		}
	}
	color o;
	o.r = -1;
	o.g = -1;
	o.b = -1;
	return o;
}