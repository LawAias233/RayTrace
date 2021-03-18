#pragma once

#include "rtweekend.h"

class Camera
{
public:
	Camera(double aspect_ratio) {
		//viewport相当于摄像机的镜头
		double viewport_height = 2.0;
		double viewport_width = aspect_ratio * viewport_height;
		double focal_length = 1.0;//焦距

		origin = Point3(0, 0, 0);
		horizontal = Vec3(viewport_width, 0, 0);
		vertical = Vec3(0, viewport_height, 0);
		lower_left_corner = origin - horizontal / 2 - vertical / 2 - Vec3(0, 0, focal_length); //我想理解成(l,b,n)
	}

	Ray get_ray(double u, double v) const 
	{
		return Ray(origin, lower_left_corner+ u * horizontal + v * vertical - origin);
	}

private:
	Point3 origin;
	Point3 lower_left_corner;
	Vec3 horizontal;
	Vec3 vertical;
};

