#include "Shapes.h"
#include <stdlib.h>     /* abs */

/* HELP FUNCTIONS  */
Vec cross(Vec u, Vec v)
{
	float e1 = u.y*v.z - v.y*u.z;
	float e2 = v.x*u.z - u.x*v.z;	// inverted to get e1 -e2 e3
	float e3 = u.x*v.y - v.x*u.y;
	Vec crossed = { e1, e2, e3 };
	return crossed;
}
float det(Vec u, Vec v, Vec w) {
	float det = 0.0f;
	Vec area = cross(v, w);
	det = u.Dot(area);
	return det;
}
Color Shape::shade(Vec& light, const Vec& cam, Ray& r, HitData& h) {

	Vec hitpoint = (r.o + r.d*h.t);				      // vector from pixel to hitpoint
	Vec liToHit = light - hitpoint;				      // Vector from lightpoint to hitpoint
	liToHit.Normalize();
	Vec hitNorm = this->normal(r.o + r.d*h.t);		// Norm from hitpoint
	hitNorm.Normalize();
	float colorCheck;
	float angle = hitNorm.Dot(liToHit);					// angle between light to hitpoint and norm
	if (angle < 0) {									// make angle stay positive
		angle = 0;
	}
	// 1.0 is the diffuse color
	colorCheck = (this->c.r * (1.0f * angle));
	if (colorCheck > 255)
		colorCheck = 255;
	h.color.r = colorCheck;

	colorCheck = (this->c.g * (1.0f * angle));
	if (colorCheck > 255)
		colorCheck = 255;
	h.color.g = colorCheck;

	colorCheck = (this->c.b * (1.0f * angle));
	if (colorCheck > 255)
		colorCheck = 255;
	h.color.b = colorCheck;

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
	Vec k = this->center - ray.o;			        // Length between sphere center and ray origin
	float s = ray.d.Dot(k);						    // k's projection onto ray.d
	float k2 = k.Dot(k);						    // k2 = k^2
	if (s <= 0 || radius2 > k2) { return; }			// Sphere is behind or if inside sphere do not hit
	float variansFromCenter = k2 - pow(s, 2);		// the length^2 between radius of sphere and s
	if (variansFromCenter > radius2) { return; }	// rejection
								 // HIT									
	float q = sqrt(radius2 - variansFromCenter);	// getting the cathetus q to find first hit t
	float t = s - q;
	if (t > 0) {                                    // if object is behind camera OR object is behind old
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
	Vec e1 = this->p2 - this->p1;
	Vec e2 = this->p3 - this->p1;
	Vec s = ray.o - this->p1;
	float detInv = 1 / det(ray.d*-1, e1, e2);
	if (detInv == 0) { return; }						// if detInv is zero there can be no triangle
	float u = detInv * det(ray.d*-1, s, e2);
	float v = detInv * det(ray.d*-1, e1, s);
	if (u < 0.0f || u > 1.0f) { return; }				// checking u and v
	if (v < 0.0f || v > 1.0f) { return; }
	float w = 1.0f - u - v;								// if they are fine, create w
	if (w < 0.f || w > 1.f) { return; }					// check w
	float t = detInv * det(s, e1, e2);					// u,v,w are fine create t, then check
	if (t > 0.f) {                                      // if object is front of camera 
		if (hit.t == -1 || t < hit.t) {					// if never been hit OR t is closer to camera 
			hit.t = t;
			hit.color = this->c;
			hit.lastNormal = this->nor;
			hit.lastShape = this;
		}
	}
	return;
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
void MyOBB::test(Ray& ray, HitData& hit) {
	float tMax = INFINITY;
	float tMin = -INFINITY;
	Vec p = this->Bcenter - ray.o;
	Vec Buvw[3] = { this->Bu,this->Bv,this->Bw };
	float halfB[3] = { this->halfBu,this->halfBv,this->halfBw };
	float e[3];
	float f[3];
	float epsilon = 0.00001f;
	float t1;
	float t2;
	for (size_t i = 0; i < 3; i++)
	{
		e[i] = Buvw[i].Dot(p);
		f[i] = Buvw[i].Dot(ray.d);

		if (abs(f[i]) > epsilon) {
			t1 = (e[i] - halfB[i]) / f[i];
			t2 = (e[i] + halfB[i]) / f[i];

			if (t1 > t2) {
				float temp = t2;
				t2 = t1;
				t1 = temp;
			}
			if (t1 > tMin) { tMin = t1; }
			if (t2 < tMax) { tMax = t2; }
			if (tMin > tMax) { return; }
			if (tMax < 0) { return; }
		}
		else if (-e[i] - halfB[i] > 0 || -e[i] + halfB[i] < 0) { return; }
	}
	if (hit.t == -1 || tMin < hit.t) {					// if never been hit OR t is closer to camera
		std::cout << "Inside" << std::endl;
		hit.t = tMin;
		hit.color = this->c;
		hit.lastNormal = this->normal(ray.d*tMin);
		hit.lastShape = this;
	}
	else if (hit.t == -1 || tMax < hit.t)
	{
		std::cout << "Inside else" << std::endl;
		hit.t = tMax;
		hit.color = this->c;
		hit.lastNormal = this->normal(ray.d*tMax);
		hit.lastShape = this;
	}

}
Vec MyOBB::normal(Vec& point) {
	bool keepSearch = true;				// search condition for searching loop
	Vec norm;
	Vec sides[6];						// points to the center of all OBBs sides
	float tests[6];						// Dot product tests with vector(side center to point) and
	float epsilon = 0.01f;
	Vec possibleHalfNorms[6] = { this->Bu * halfBu, this->Bu *-halfBu, 
							this->Bv * halfBv, this->Bv *-halfBv,
							this->Bw * halfBw, this->Bw *-halfBw};
	for (size_t i = 0; i < 6; i++)
	{
		sides[i] = this->Bcenter + possibleHalfNorms[i];		// creating each side of OBB
		tests[i] = possibleHalfNorms[i].Dot(sides[i] - point);	// testing current normal if
		if (tests[i] > -epsilon && tests[i] < epsilon) {
			norm = possibleHalfNorms[i];
			norm.Normalize();
			keepSearch = false;
		}
	}
	
	return norm;
}

// Center point, lenght U vector, length V vector, length W vector, color

MyOBB::MyOBB(Vec b, Vec b1, Vec b2, Vec b3, float Hu, float Hv, float Hw, Color _color) {
	this->Bu = b1;
	this->Bv = b2;
	this->Bw = b3;
	this->Bcenter = b;
	this->halfBu = Hu;
	this->halfBv = Hv;
	this->halfBw = Hw;
	this->c = _color;
	this->Pu = Bu - Bcenter;
	this->Pv = Bv - Bcenter;
	this->Pw = Bw - Bcenter;
}
//MyOBB::MyOBB(Vec b, float Hu, float Hv, float Hw, Color _color) {}
