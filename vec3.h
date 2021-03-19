#pragma once

#include "rtweekend.h"

#include <math.h>
#include <iostream>
class Vec3;

typedef Vec3 Point3;
typedef Vec3 Color;

class Vec3
{
public:
	double d[3];

	Vec3() : d{ 0,0,0 } {}
	Vec3(double _d) : d{ _d, _d, _d } {}
	Vec3(double x, double y, double z) : d{ x,y,z } {}
	
	double x() { return d[0]; }
	double y() { return d[1]; }
	double z() { return d[2]; }

	Vec3 operator-() const { return Vec3(-d[0], -d[1], -d[2]); }
	double operator[](int i) const { return d[i]; }
	double& operator[](int i) { return d[i]; }
	
	Vec3& operator+=(const Vec3& _v) {
		d[0] += _v.d[0];
		d[1] += _v.d[1];
		d[2] += _v.d[2];
		return *this;
	}

	Vec3& operator*=(double t){
		d[0] *= t;
		d[1] *= t;
		d[2] *= t;
		return *this;
	}

	Vec3& operator/=(double t){
		//return *this *= 1 / t; //这个优先级绝了我还是加括号吧
		return ((*this) *= (1 / t)); 
	}

	double length_squared() const
	{
		return d[0] * d[0] + d[1] * d[1] + d[2] * d[2];
	}

	double length() const {
		return std::sqrt(length_squared());
	}

	bool near_zero() const {
		// Return true if the vector is close to zero in all dimensions.
		const auto s = 1e-8;
		return (fabs(d[0]) < s) && (fabs(d[1]) < s) && (fabs(d[2]) < s);
	}

	inline static Vec3 random()
	{
		return Vec3(random_double(), random_double(), random_double());
	}
	
	inline static Vec3 random(double min, double max)
	{
		return Vec3(random_double(min,max), random_double(min,max), random_double(min,max));
	}
};

inline std::ostream& operator<<(std::ostream& out, const Vec3& v)
{
	return out << v.d[0] << ' ' << v.d[1] << ' ' << v.d[2];
}

inline Vec3 operator+(const Vec3& u, const Vec3& v)
{
	return Vec3(u.d[0] + v.d[0], u.d[1] + v.d[1], u.d[2] + v.d[2]);
}

inline Vec3 operator-(const Vec3& u, const Vec3& v)
{
	return Vec3(u.d[0] - v.d[0], u.d[1] - v.d[1], u.d[2] - v.d[2]);
}

inline Vec3 operator*(const Vec3& u, const Vec3& v)
{
	return Vec3(u.d[0] * v.d[0], u.d[1] * v.d[1], u.d[2] * v.d[2]);
}

inline Vec3 operator*(const Vec3& u, double t)
{
	return Vec3(u.d[0] * t, u.d[1] * t, u.d[2] * t);
}

inline Vec3 operator*(double t, const Vec3& u)
{
	return u * t;
}

inline Vec3 operator/(const Vec3& u, double t)
{
	return u * (1 / t);
}

inline double dot(const Vec3& u, const Vec3& v)
{
	return u.d[0] * v.d[0] + u.d[1] * v.d[1] + u.d[2] * v.d[2];
}

inline Vec3 cross(const Vec3& u, const Vec3& v)
{
	return Vec3(
		u.d[1] * v.d[2] - u.d[2] * v.d[1],
		u.d[2] * v.d[0] - u.d[0] * v.d[2],
		u.d[0] * v.d[1] - u.d[1] * v.d[0]
	);
}

inline Vec3 normalized(Vec3 v)
{
	return v / v.length();
}

Vec3 random_in_unit_sphere() {
	while (true) {
		auto p = Vec3::random(-1, 1);
		if (p.length_squared() >= 1) continue;
		return p;
	}
}

Vec3 random_unit_vector()
{
	return normalized(random_in_unit_sphere());
}

Vec3 random_in_hemisphere(const Vec3& normal) {
	Vec3 in_unit_sphere = random_in_unit_sphere();
	if (dot(in_unit_sphere, normal) > 0.0) // In the same hemisphere as the normal
		return in_unit_sphere;
	else
		return -in_unit_sphere;
}

Vec3 reflect(const Vec3& v, const Vec3& n)
{
	return v - 2 * n * dot(v, n);
}

inline void output_color(std::ostream& out, Color cor, int samples_per_pixel)
{
	auto r = cor.x();
	auto g = cor.y();
	auto b = cor.z();

	auto scale = 1.0 / samples_per_pixel;
	r *= scale; 
	g *= scale;
	b *= scale;

	out << static_cast<int>(255.99 * r) << ' '
		<< static_cast<int>(255.99 * g) << ' '
		<< static_cast<int>(255.99 * b) << '\n';
}


inline int vec3_Test()
{
	Vec3 v1, v2(3), v3(2, 3, 4), v4(v3);
	std::cout << v1 << std::endl;
	std::cout << v2 << std::endl;
	std::cout << v3 << std::endl;
	std::cout << v4 << std::endl;
	std::cout << v2 + v3 << std::endl;
	std::cout << v3 - v2 << std::endl;
	std::cout << v3 / 2 << std::endl;
	std::cout << v3 * 2 << std::endl;
	std::cout << 2 * v3 << std::endl;
	std::cout << dot(v2, v3) << std::endl;
	std::cout << cross(Vec3(0, 0, 1), Vec3(0, 1, 0)) << std::endl;
	return 0;
}