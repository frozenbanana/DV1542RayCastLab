#include "Shapes.h"

Color Shape::shade(Vec& light, const Vec& cam, Ray& r, HitData& h) {
	//float rgb[3];
	//Color result(rgb[0], rgb[1], rgb[2]);
	Vec hitpoint = (r.o + r.d*h.t);				      // vector from pixel to hitpoint
	Vec liToHit = light - hitpoint;				      // Vector from lightpoint to hitpoint
	liToHit.Normalize();
	Vec hitNorm = this->normal(r.o + r.d*h.t);		// Norm from hitpoint
	hitNorm.Normalize();
	float angle = hitNorm.Dot(liToHit);					// angle between light to hitpoint and norm
	if (angle < 0) {									// make angle stay positive
		angle = 0;
	}
							// 1.0 is the diffuse color
	h.color.r = (this->c.r * (1.0 * angle));
	h.color.b = (this->c.b * (1.0 * angle));
	h.color.g = (this->c.g * (1.0 * angle));
	//if (rgb[0] > 255) { rgb[0] = 255; };
	//if (rgb[1] > 255) { rgb[1] = 255; };
	//if (rgb[2] > 255) { rgb[2] = 255; };

	return h.color;
}

void MyPlane::test(Ray& ray, HitData& hit) {
	Vec p = { 0,0,1000 };											         // point inside plane
	float t = -this->n.Dot(ray.o) / this->n.Dot(ray.d) - this->d;	// calc t were ray and plane intersect
	Vec td(t*ray.d.x, t*ray.d.y, t*ray.d.z);
	Vec testHit = ray.o + td;										         // testHit is going in plane to be 0
	if (testHit.Dot(this->n) == 0) {
		return;
	}
	if (t > 0) {                                      // if object is behind camera OR object is behind old
		if (hit.t == -1 || t < hit.t)
		{
			hit.t = t;
			hit.color = this->c;
			hit.lastNormal = this->n;
			hit.lastShape = this;
		}
	}
	return;
}
Vec MyPlane::normal(Vec &point) {
	this->n.Normalize();
	return this->n;
}
MyPlane::MyPlane(Vec normal, float _d, Color color) {
	this->n = normal;
	this->d = _d;
	this->c = color;
}
/* SPHERE */
MySphere::MySphere(Vec center, float radius, Color color) {
	this->center = center;
	this->radius = radius;
	this->c = color;
	this->radius2 = pow(radius, 2);
}

void MySphere::test(Ray& ray, HitData& hit) {
	Vec k = this->center - ray.o;				      // Length between sphere center and ray origin
	float s = ray.d.Dot(k);						      // k's projection onto ray.d
	float k2 = k.Dot(k);						         // k2 = k^2
	if (s <= 0 || radius2 > k2) { return; }		// Sphere is behind or if inside sphere do not hit
	float variansFromCenter = k2 - pow(s, 2);	   // the length^2 between radius of sphere and s
	if (variansFromCenter > radius2) { return; } // rejection
												 // HIT									
	float q = sqrt(radius2 - variansFromCenter);		// getting the cathetus q to find first hit t
	float t = s - q;
	if (t > 0) {                                      // if object is behind camera OR object is behind old
		if (hit.t == -1 || t < hit.t) {
			hit.t = t;
			hit.color = this->c;
			hit.lastNormal = k;
			hit.lastShape = this;
		}
	}
}

Vec MySphere::normal(Vec &point) {
	// What happens if point is NULL?
	Vec nor;
	nor = point - center;
	nor.Normalize();
	return nor;
}

/* TRIANGLE */
void MyTriangle::test(Ray& ray, HitData& hit) {
	// TO DO
}
Vec MyTriangle::normal(Vec &point) {
	return this->nor;
}
MyTriangle::MyTriangle(Vec _p1, Vec _p2, Vec _p3, Color _color) {
	this->c = _color;
	this->p1 = _p1;
	this->p2 = _p2;
	this->p3 = _p3;
	Vec u = p2 - p1;
	Vec v = p3 - p1;
	this->nor = cross(u,v);
}

/*    OBB    */
void MyOBB::test(Ray& ray, HitData& hit);
Vec MyOBB::normal(Vec& point) {
	// TODO
}

// Center point, lenght U vector, length V vector, length W vector, color

MyOBB::MyOBB(Vec b, Vec b1, Vec b2, Vec b3, float Hu, float Hv, float Hw, Color _color) {
	// TODO
}
MyOBB::MyOBB(Vec b, float Hu, float Hv, float Hw, Color _color) {
	// TODO
}