#define STB_IMAGE_WRITE_IMPLEMENTATION 

#include "rtweekend.h"
#include "Hittable_list.h"
#include "Sphere.h"
#include "Camera.h"

#include "stb_image_write.h"

#include <iostream>
#include <stdio.h>

Color ray_color(const Ray& r, const Hittable& world);

int main()
{
	//Image
	const auto aspect_ratio = 4.0 / 3.0;
	const int image_width(800);
	const int image_height(image_width/aspect_ratio);
	const int cmp(3);
	int dataIndex(0);
	std::auto_ptr<unsigned char> data(new unsigned char[image_height * image_width * cmp]);
	std::cout << "weight:" << image_width << ",height:" << image_height << std::endl;

	//World
	Hittable_list world;
	world.add(std::make_shared<Sphere>(Vec3(0,0,-1), 0.3));
	world.add(std::make_shared<Sphere>(Vec3(0,-100.5,0), 100));

	//Camera
	Camera cam(aspect_ratio);
	
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

				Color pixel_color = ray_color(r, world);

				data.get()[dataIndex + 0] = pixel_color.x() * 255.99;
				data.get()[dataIndex + 1] = pixel_color.y() * 255.99;
				data.get()[dataIndex + 2] = pixel_color.z() * 255.99;
				dataIndex += cmp;
			}
		}
	}
	else if (aa == 1)
	{//RandomAA
		int Random_sample_pixel = 4;
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
					pixel_color += ray_color(r, world);
				}
				double scale = 1.0/Random_sample_pixel;
				data.get()[dataIndex + 0] = scale * pixel_color.x() * 255.99;
				data.get()[dataIndex + 1] = scale * pixel_color.y() * 255.99;
				data.get()[dataIndex + 2] = scale * pixel_color.z() * 255.99;
				dataIndex += cmp;
			}
		}
	}
	

	stbi_write_png("output//out.png", image_width, image_height, cmp, data.get(), image_width*cmp);
	return 0;
}



Color ray_color(const Ray& r, const Hittable& world)
{
	hit_record rec;
	if(world.hit(r,0,infinity, rec))
	{
		return 0.5 * (rec.normal + Color(1, 1, 1));
	}

	Vec3 unit_direction = normalized(r.direction());
	auto t = 0.5 * (unit_direction.y() + 1.0);
	return (1.0 - t) * Color(1.0, 1.0, 1.0) + t * Color(0.5, 0.7, 1.0);
}