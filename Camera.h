#pragma once

#include "rtweekend.h"

class Camera
{
public:
	Camera(
		Point3 lookFrom,
		Point3 lookAt,
		Point3 cameraUp,
		double fov,
		double aspect_ratio) {
		//viewport相当于摄像机的镜头
		double theta = degrees_to_radians(fov);
		double h = tan(theta / 2);
		double viewport_height = 2.0 * h;
		double viewport_width = aspect_ratio * viewport_height;
		double focal_length = 1.0;//焦距

		Vec3 w = normalized(lookFrom - lookAt);
		Vec3 u = normalized(cross(cameraUp, w));
		Vec3 v = cross(w,u);


		origin = lookFrom;
		horizontal = viewport_width * u;
		vertical = viewport_height * v;
		//origin = Point3(0, 0, 0);
		//horizontal = Vec3(viewport_width, 0, 0);
		//vertical = Vec3(0, viewport_height, 0);
		lower_left_corner = origin - horizontal / 2 - vertical / 2 - w; //我想理解成(l,b,n)
	}

	Ray get_ray(double s, double t) const 
	{
		return Ray(origin, lower_left_corner+ s * horizontal + t * vertical - origin);
	}

private:
	Point3 origin;
	Point3 lower_left_corner;
	Vec3 horizontal;
	Vec3 vertical;
};

