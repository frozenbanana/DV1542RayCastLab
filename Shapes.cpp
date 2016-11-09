#include "Shapes.h"

Color Shape::shade(Vec& light, const Vec& cam, Ray& r, HitData& h) {
	Vec hitpoint = (r.o + r.d*h.t);				 // vector from pixel to hitpoint
	Vec liToHit = light - hitpoint;				// Vector from lightpoint to hitpoint
	liToHit.Normalize();
	Vec hitNorm = this->normal(r.o + r.d*h.t);		// Norm from hitpoint
	float angle = hitNorm.Dot(hitpoint);					// angle between vector and norm
							// 1.0 is the diffuse color
	h.color.r = (this->c.r * (1.0 * angle));
	h.color.b = (this->c.b * (1.0 * angle));
	h.color.g = (this->c.g * (1.0 * angle));

	return h.color;
}
void MyPlane::test(Ray& ray, HitData& hit) {
	Vec p = { 0,0,1000 };											         // point inside plane
	float t = -this->n.Dot(ray.o) / this->n.Dot(ray.d) - this->d;	// calc t were ray and plane intersect
	Vec td(t*ray.d.x, t*ray.d.y, t*ray.d.z);
	Vec testHit = ray.o + td;										         // testHit is going in plane to be 0
	if (testHit.Dot(this->n) != 0) {
		return;
	}
   if (t < 0 || t > hit.t) {                                      // if object is behind camera OR object is behind old
      return;
   }
	else{
		// HIT
		hit.t = t;
		hit.color = this->c;
		hit.lastNormal = this->n;
		hit.lastShape = this;
	}
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
// Create your own subclasses of Shape, for Sphere, Triangle, OBB and Plane.
MySphere::MySphere(Vec center, float radius, Color color) {
	this->center = center;
	this->radius = radius;
	this->c = color;
	this->radius2 = pow(radius, 2);
}

void MySphere::test(Ray& ray, HitData& hit) {
	Vec k = this->center - ray.o;				// Length between sphere center and ray origin
	float s = ray.d.Dot(k);						// k's projection onto ray.d
	float k2 = k.Dot(k);						   // k2 = k^2
	if (s <= 0 && radius2 < k2) { return; }		// if inside sphere do not hit
	float variansFromCenter = k2 - pow(s, 2);	// the length^2 between radius of sphere and s
	if (variansFromCenter > radius2) {	return;}// rejection
	else {													// HIT
		float q = sqrt(radius2 - variansFromCenter);		// getting the cathetus q to find first hit t
		hit.t = s-q;
		hit.color = this->c;
		hit.lastNormal = k;
		hit.lastShape = this;
	}
}

Vec MySphere::normal(Vec &point) {
	// What happens if point is NULL?
	Vec nor;
	nor = point - center;
	nor.Normalize();
	return nor;
}


/*class MyTriangle : public Shape
{
	Vec center;
	float radius;
	float radius2;
public:
	void test(Ray& ray, HitData& hit);
	Vec normal(Vec &point);
	MySphere(Vec _center, float _radius, Color _color);
};
*/