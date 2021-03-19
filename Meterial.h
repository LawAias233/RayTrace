#pragma once

#include "rtweekend.h"

struct hit_record;

class Material
{
public:
    virtual bool scatter(
        const Ray& r_in, const hit_record& rec, Color& attenuation, Ray& scattered
    ) const = 0;
};

class Lambertian : public Material {
public:
    Lambertian(const Color& a) : albedo(a) {}

    virtual bool scatter(
        const Ray& r_in, const hit_record& rec, Color& attenuation, Ray& scattered
    ) const override {
        //取随机反射方向，但是没有考虑到法向量的方向，将会出现二者方向相反
        //于是可能出现scatter_direction非常接近零向量的情况
        //最终导致Nan与Inf
        auto scatter_direction = rec.normal + random_unit_vector(); 
        if (scatter_direction.near_zero())
            scatter_direction = rec.normal;
        scattered = Ray(rec.p, scatter_direction);
        attenuation = albedo;
        return true;
    }

public:
    Color albedo;
};

class Metal : public Material {
public:
    Metal(const Color& a, float f) : albedo(a), fuzz(f > 1 ? 1 : f){}

    virtual bool scatter(
        const Ray& r_in, const hit_record& rec, Color& attenuation, Ray& scattered
    ) const override {
        Vec3 reflected = reflect(normalized(r_in.direction()), rec.normal);
        //scattered = Ray(rec.p, reflected); //镜面反射，但金属实际上有凹槽会模糊
        scattered = Ray(rec.p, reflected + fuzz*random_in_unit_sphere());
        attenuation = albedo;
        return (dot(scattered.direction(), rec.normal) > 0);
    }

public:
    Color albedo;
    float fuzz;
};