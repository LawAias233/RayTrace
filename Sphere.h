#pragma once
#include "Hittable.h"
class Sphere :
    public Hittable
{
public:
    Sphere():center(0),radius(0){}
    Sphere(Point3 cen, double r) :center(cen), radius(r) {}

    virtual bool hit(
        const Ray& r, double t_min, double t_max, hit_record& rec) const override;

public:
    Point3 center;
    double radius;
};

bool Sphere::hit(const Ray& r, double t_min, double t_max, hit_record& rec) const
{
	double a = dot(r.direction(), r.direction());
	double half_b = dot(r.direction(), (r.origin - center));
	double c = dot((r.origin - center), (r.origin - center)) - radius * radius;
	double discriminant = half_b * half_b - a * c;
	if (discriminant < 0) return false;
	double sqrtd = sqrt(discriminant);

	auto root = (-half_b - sqrtd) / a;
	if (root < t_min || root > t_max)
	{
		root = (-half_b + sqrtd) / a;
		if (root < t_min || root > t_max)
			return false;
	}

	rec.t = root;
	rec.p = r.at(rec.t);
	Vec3 outward_normal = (rec.p - center) / radius;
	rec.set_face_normal(r, outward_normal);

	return true;
}

//ÒÑ·ÏÆú
double hit_sphere(const Point3& center, double radius, const Ray& r)
{
	double a = dot(r.direction(), r.direction());
	double half_b = dot(r.direction(), (r.origin - center));
	double c = dot((r.origin - center), (r.origin - center)) - radius * radius;
	double square_root = half_b * half_b - a * c;
	if (square_root < 0)
		return -1.0f;
	else
	{
		return (-half_b - sqrt(square_root)) / a;
	}
}