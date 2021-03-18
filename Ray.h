#pragma once

#include "vec3.h"

class Ray
{
public:
	Point3 origin;
	Vec3 dir;
public:
	Ray() {}
	Ray(const Point3& _origin, const Vec3& _direction):origin(_origin),dir(_direction){}

	Point3 oring() const { return origin; }
	Vec3 direction() const { return dir; }

	Point3 at(double t) const {
		return origin + t * dir;
	}
};

