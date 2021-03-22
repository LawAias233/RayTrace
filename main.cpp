#define STB_IMAGE_WRITE_IMPLEMENTATION 

#include "rtweekend.h"
#include "Hittable_list.h"
#include "Sphere.h"
#include "Camera.h"
#include "Meterial.h"

#include "stb_image_write.h"

#include <iostream>
#include <stdio.h>

Color ray_color(const Ray& r, const Hittable& world, int depth);
inline void UpdateProgress(float progress);

int depthCount[50];

int main()
{
	//Image
	const auto aspect_ratio = 4.0 / 3.0;
	const int image_width(800);
	const int image_height(image_width/aspect_ratio);
	const int cmp(3);
	const int maxDepth(50);
	int dataIndex(0);
	std::auto_ptr<unsigned char> data(new unsigned char[image_height * image_width * cmp]);
	std::cout << "weight:" << image_width << ",height:" << image_height << std::endl;

	//World
	Hittable_list world;

	auto material_ground = std::make_shared<Lambertian>(Color(0.8, 0.8, 0.0));
	auto material_center = std::make_shared<Lambertian>(Color(0.1, 0.2, 0.5));
	auto material_left = std::make_shared<Dielectric>(1.5);
	auto material_right = std::make_shared<Metal>(Color(0.8, 0.6, 0.2), 0.0);
	//auto material_123 = std::make_shared<Metal>(Color(0.8, 0.6, 0.2), 0.0);

	world.add(std::make_shared<Sphere>(Point3(0.0, -100.5, -1.0), 100.0, material_ground));
	world.add(std::make_shared<Sphere>(Point3(0.0, 0.0, -1.0), 0.5, material_center));
	world.add(std::make_shared<Sphere>(Point3(-1.0, 0.0, -1.0), 0.5, material_left));
	world.add(std::make_shared<Sphere>(Point3(-1.0, 0.0, -1.0), -0.45, material_left));
	world.add(std::make_shared<Sphere>(Point3(1.0, 0.0, -1.0), 0.5, material_right));
	//world.add(std::make_shared<Sphere>(Point3(0.0, 0.0, -1.0), 0.5, material_123));

	//Camera
	Camera cam(Point3(-2, 2, 1), Point3(0, 0, -1), Vec3(0, 1, 0), 90, aspect_ratio);
	//Camera cam(Point3(0, 0, 1), Point3(0, 0, -1), Vec3(0, 1, 0), 90, aspect_ratio);
	
	//Render
	int aa(0);
	aa = 1;
	if (aa == 0)
	{//without AA
		for (int i = image_height - 1; i >= 0; --i)
		{
			for (int j = 0; j < image_width; ++j)
			{
				//Color color(double(i) / (image_width - 1), double(j) / (image_height - 1), 0.25);
				auto u = double(j) / (image_width - 1);
				auto v = double(i) / (image_height - 1);

				Ray r = cam.get_ray(u, v);

				Color pixel_color = ray_color(r, world, maxDepth);

				data.get()[dataIndex + 0] = pixel_color.x() * 255.99;
				data.get()[dataIndex + 1] = pixel_color.y() * 255.99;
				data.get()[dataIndex + 2] = pixel_color.z() * 255.99;
				dataIndex += cmp;
			}
		}
	}
	else if (aa == 1)
	{//RandomAA
		int Random_sample_pixel = 50;
		for (int i = image_height - 1; i >= 0; --i)
		{
			for (int j = 0; j < image_width; ++j)
			{
				Color pixel_color(0, 0, 0);
				for (int k = 0; k < Random_sample_pixel; ++k)
				{
					auto u = (j+random_double()) / (image_width - 1);
					auto v = (i+random_double()) / (image_height - 1);
					Ray r = cam.get_ray(u, v);
					pixel_color += ray_color(r, world, maxDepth);
				}
				double scale = 1.0/Random_sample_pixel;
				data.get()[dataIndex + 0] = sqrt(scale * pixel_color.x()) * 255.99;
				data.get()[dataIndex + 1] = sqrt(scale * pixel_color.y()) * 255.99;
				data.get()[dataIndex + 2] = sqrt(scale * pixel_color.z()) * 255.99;
				dataIndex += cmp;
			}
			UpdateProgress((float)(image_height-i)/image_height);
		}
		UpdateProgress(1.0f);
	}

	stbi_write_png("output//out.png", image_width, image_height, cmp, data.get(), image_width*cmp);
	return 0;
}



Color ray_color(const Ray& r, const Hittable& world, int depth)
{
	++depthCount[depth-1];
	if (depth <= 0) return Color(0.0f, 0.0f, 0.0f);
	hit_record rec;
	if(world.hit(r,0.001,infinity, rec))
	{
		//Point3 target = rec.p + rec.normal + random_in_unit_sphere();
		//Point3 target = rec.p + rec.normal + random_unit_vector();
		//Point3 target = rec.p + rec.normal + random_in_hemisphere(rec.normal);
		//return 0.5 * ray_color(Ray(rec.p, target - rec.p), world,depth-1);
		//return 0.5 * (rec.normal + Color(1, 1, 1));
		Color attenuation;
		Ray scattered;
		if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
		{
			return attenuation * ray_color(scattered, world, depth - 1);
		}
		else
		{
			return Color(0.0f, 0.0f, 0.0f);
		}
	}

	Vec3 unit_direction = normalized(r.direction());
	auto t = 0.5 * (unit_direction.y() + 1.0);
	return (1.0 - t) * Color(1.0, 1.0, 1.0) + t * Color(0.5, 0.7, 1.0);
}

inline void UpdateProgress(float progress)
{
	int barWidth = 70;

	std::cout << "[";
	int pos = barWidth * progress;
	for (int i = 0; i < barWidth; ++i) {
		if (i < pos) std::cout << "=";
		else if (i == pos) std::cout << ">";
		else std::cout << " ";
	}
	std::cout << "] " << int(progress * 100.0) << " %\r";
	std::cout.flush();
};