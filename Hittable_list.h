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

