# 射线

## 1.About

## 2.Ray

## 3.Ray2Scene

从摄像机发出一条射线，穿过某一像素(从而得到dir)，计算最终颜色。

1. 计算从摄像机到像素的方向
2. 判断是否有物体与光线相交
3. 计算颜色

为了方便调整x轴和y轴，将图片分辨率设为非正方形。

```c++
const auto aspect_ratio = 4.0 / 3.0;
	const int image_width(800);
	const int image_height(image_width/aspect_ratio);
	const int cmp(3);
	int dataIndex(0);
	std::auto_ptr<unsigned char> data(new unsigned char[image_height * image_width * cmp]);
	std::cout << "weight:" << image_width << ",height:" << image_height << std::endl;
```

为了模拟相机，设置一个虚拟的视口`viewport`平面

`lower_left_corner`比较特别和有趣，它指的是，该视口平面的左下角那个点。因为视口在`-z`方向上，所以减去`Vec3(0, 0, focal_length)`

```c++
//Camera
	//viewport相当于摄像机的镜头
	double viewport_height = 2.0;
	double viewport_width = aspect_ratio * viewport_height;
	double focal_length = 1.0;//焦距

	Point3 origin = Point3(0, 0, 0);
	Vec3 horizontal = Vec3(viewport_width, 0, 0);
	Vec3 vertical = Vec3(0, viewport_height, 0);
	Vec3 lower_left_corner = origin - horizontal / 2 - vertical / 2 - Vec3(0, 0, focal_length); //我想理解成(l,b,n)
```

最后的渲染变化不大

```c++
//Color color(double(i) / (image_width - 1), double(j) / (image_height - 1), 0.25);
		auto u = double(i) / (image_width - 1);
		auto v = double(j) / (image_height - 1);
		Ray r(origin, lower_left_corner + u * horizontal + v * vertical - origin);
		Color pixel_color = ray_color(r);

		data.get()[dataIndex+0] = pixel_color.x()*255.99;
		data.get()[dataIndex+1] = pixel_color.y()*255.99;
		data.get()[dataIndex+2] = pixel_color.z()*255.99;
		dataIndex += cmp;
```

关键是`ray_color`函数，这里是指定了在y轴上

```c++
Color ray_color(const Ray& r)
{
	Vec3 unit_direction = normalized(r.direction());
	auto t = 0.5 * (unit_direction.y() + 1.0);
	return (1.0 - t) * Color(1.0, 1.0, 1.0) + t * Color(0.5, 0.7, 1.0);
}
```

这里是在根据y轴做蓝色blue`(0.5,0.7,1.0)`的线性插值

### 3.1 绘制一个圆

返回光线r击中圆时的交点情况

```c++
double hit_sphere(const Point3& center, double radius, const Ray& r)
{
	double a = dot(r.direction(), r.direction());
	double b = 2 * dot( r.direction(),(r.origin - center));
	double c = dot((r.origin-center), (r.origin-center)) - radius*radius;
	double square_root = b * b - 4 * a * c;
	if (square_root < 0)
		return -1.0f;
	else
	{
		return (-b - sqrt(square_root)) / (2.0 * a);
	}
```

调用`hit_sphere`

```c++
Color ray_color(const Ray& r)
{
	auto dis = hit_sphere(Vec3(0, 0, -1), 0.5, r);
	if (dis > 0)
	{
		Vec3 N = normalized(r.at(dis) - Vec3(0, 0, -1));
		return 0.3 * Color(N.x() + 1, N.y() + 1, N.z() + 1);
	}

	Vec3 unit_direction = normalized(r.direction());
	auto t = 0.5 * (unit_direction.y() + 1.0);
	return (1.0 - t) * Color(1.0, 1.0, 1.0) + t * Color(0.5, 0.7, 1.0);
}
```

注意原先的计算有些复杂，若令 **h** = **b**/2则有
$$
\frac{-b \pm \sqrt{b^{2}-4ac}}{2a}变成\frac{-2h\pm2\sqrt{h^{2}-ac}}{2a}即\frac{-h\pm\sqrt{h^{2}-ac}}{a}
$$
于是得新的`hit_sphere`计算

```c++
double hit_sphere(const Point3& center, double radius, const Ray& r)
{
	double a = dot(r.direction(), r.direction());
	double half_b = dot( r.direction(),(r.origin - center));
	double c = dot((r.origin-center), (r.origin-center)) - radius*radius;
	double square_root = half_b*half_b - a * c;
	if (square_root < 0)
		return -1.0f;
	else
	{
		return (-half_b - sqrt(square_root)) / a;
	}
}
```

### 3.2 绘制多个圆

将3.1的代码进行抽象，使其支持绘制多个圆。

```c++
#pragma once
#include "Hittable.h"
class Sphere :
    public Hittable
{
public:
    Sphere() {}
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
```

显然，接下来，只需要在`ray_color`时查找所有的物体，判断是否相交即可。

那么自然而然，为了遍历的方便，建立一个存储所有Object是数据结构十分有必要。

### 3.3 ObjectsList

```c++
#pragma once
#include "Hittable.h"

#include <memory>
#include <vector>

class Hittable_list :
    public Hittable
{
public:
    Hittable_list(){}
    Hittable_list(std::shared_ptr<Hittable> object) { add(object); }

    void clear() { objects.clear(); }
    void add(std::shared_ptr<Hittable> object) { objects.push_back(object); }

    virtual bool hit(
        const Ray& r, double t_min, double t_max, hit_record& rec) const override;
public:
    std::vector<std::shared_ptr<Hittable>> objects;
};

bool Hittable_list::hit(const Ray& r, double t_min, double t_max, hit_record& rec)const 
{
    hit_record temp_rec;
    bool hit_anything = false;
    double closest_so_fao = t_max;

    for (const auto& object : objects)
    {
        if (object->hit(r, t_min, closest_so_fao, temp_rec))
        {
            hit_anything = true;
            closest_so_fao = temp_rec.t;
            rec = temp_rec;
        }
    }

    return hit_anything;
}


```

之后只需要修改`ray_color`添加对其的调用

```c++

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
```

为了显示多个圆形，只需要在`Hittable`中添加物体即可。