#pragma once

#include "ray.h"
#include <memory>
#include <vector>
#include "glm/glm.hpp"
#include "aabb.h"

using std::shared_ptr;
using std::make_shared;
using std::vector;

class material;


struct hit_record {
    glm::vec3 p;
    glm::vec3 normal;
    shared_ptr<material> pMat;
    double t;
    float u;
    float v;
    bool front_face;

	void set_face_normal(const ray& r, const glm::vec3& outward_normal) {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class hittable {
public:
    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const = 0;
    virtual bool boundingBox(float t0, float t1, aabb& outBox) const = 0;
};


aabb surrounding_box(aabb box0, aabb box1);
inline bool box_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b, int axis);
bool box_x_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b);
bool box_y_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b);
bool box_z_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b);

class sphere: public hittable
{
public:
    sphere(const glm::vec3&, double, shared_ptr<material>);
    virtual bool hit(const ray&, double, double, hit_record&) const override;
    bool boundingBox(float t0, float t1, aabb& outBox) const override;
protected:
    glm::vec3 center;
    double radius;
    shared_ptr<material> pMat;
    static void get_sphere_uv(const glm::vec3& p, float& u, float& v);
};

inline sphere::sphere(const glm::vec3& c, double r, shared_ptr<material> pm)
		: center(c), radius(r), pMat(pm) {}

inline bool sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const
{
    glm::vec3 oc = r.origin() - center;
    auto a = glm::dot(r.direction(), r.direction());
	auto halfB = glm::dot(oc, r.direction());
    auto c = glm::dot(oc, oc) - radius * radius;
    auto discriminant = halfB * halfB - a * c;
    if (discriminant < 0) { return false; }
    auto sqrtd = sqrt(discriminant);
    auto root = (-halfB - sqrtd) / a;

    if (root < t_min || t_max < root) {
        root = (-halfB + sqrtd) / a;
        if (root < t_min || t_max < root)
            return false;
    }
    rec.t = root;
    rec.p = r.at(root);
    glm::vec3 outward_normal = (rec.p - center) / static_cast<float>(radius);
    rec.set_face_normal(r, outward_normal);
    get_sphere_uv(outward_normal, rec.u, rec.v);
    rec.pMat = pMat;
    return true;
}

inline bool sphere::boundingBox(float t0, float t1, aabb& outBox) const
{
    outBox = aabb(center - glm::vec3(radius), center + glm::vec3(radius));
    return true;
}

inline void sphere::get_sphere_uv(const glm::vec3& p, float& u, float& v)
{
    float theta = acos(-p.y);
    float phi = atan2(-p.z, p.x) + glm::pi<float>();

    u = phi / (2.f * glm::pi<float>());
    v = theta / glm::pi<float>();
}

class hittable_list : public hittable
{
public:
    hittable_list() = default;
    hittable_list(shared_ptr<hittable> obj) { add(obj); }
    vector<shared_ptr<hittable>> getObjects() const;
    size_t size() const { return objects.size(); }
	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
    bool boundingBox(float t0, float t1, aabb& outBox) const override;
    void add(shared_ptr<hittable> obj) { objects.push_back(obj); }
    void clear() { objects.clear(); }
private:
    vector<shared_ptr<hittable>> objects;
};

inline vector<shared_ptr<hittable>> hittable_list::getObjects() const
{
    return objects;
}

inline bool hittable_list::hit(const ray& r, double t_min, double t_max, hit_record& rec) const
{
    bool hitAnything = false;
    hit_record tempRecord;
    double far = t_max;
	for(auto obj : objects)
	{
		if(obj->hit(r, t_min, far, tempRecord))
		{
            hitAnything = true;
            rec = tempRecord;
            far = tempRecord.t;
		}
	}
    return hitAnything;
}

inline bool hittable_list::boundingBox(float t0, float t1, aabb& outBox) const
{
    if (objects.empty()) return false;
    aabb tempBox(glm::vec3(0), glm::vec3(0));
    bool firstBox = true;
    for(const auto& object : objects)
    {
        if (!object->boundingBox(t0, t1, tempBox)) return false;
        outBox = firstBox ? tempBox : surrounding_box(tempBox, outBox);
        firstBox = false;
    }
    return true;
}

class movingsphere : public hittable
{
public:
    movingsphere(const glm::vec3& c0, const glm::vec3& c1, float t0, float t1, double r, shared_ptr<material> mat);
    virtual bool hit(const ray& r, double tMin, double tMax, hit_record&) const override;
    bool boundingBox(float t0, float t1, aabb& outBox) const override;
    glm::vec3 getCenter(float t)const;
protected:
    glm::vec3 center0, center1;
    float time0, time1;
    double radius;
    shared_ptr<material> pMat;
};

inline movingsphere::movingsphere(const glm::vec3& c0, const glm::vec3& c1, float t0, float t1, double r, shared_ptr<material> mat)
	: center0(c0), center1(c1), time0(t0), time1(t1), radius(r), pMat(std::move(mat)) {}

inline glm::vec3 movingsphere::getCenter(float t) const
{
    return center0 + ((t - time0) / (time1 - time0)) * (center1 - center0);
}

inline bool movingsphere::hit(const ray& r, double tMin, double tMax, hit_record& rec) const
{
    glm::vec3 center = getCenter(r.time());
    glm::vec3 oc = r.origin() - center;
    auto a = glm::dot(r.direction(), r.direction());
    auto halfB = glm::dot(oc, r.direction());
    auto c = glm::dot(oc, oc) - radius * radius;
    auto discriminant = halfB * halfB - a * c;
    if (discriminant < 0) { return false; }
    auto sqrtd = sqrt(discriminant);
    auto root = (-halfB - sqrtd) / a;

    if (root < tMin || tMax < root) {
        root = (-halfB + sqrtd) / a;
        if (root < tMin || tMax < root)
            return false;
    }
    rec.t = root;
    rec.p = r.at(root);
    glm::vec3 outward_normal = (rec.p - center) / static_cast<float>(radius);
    rec.set_face_normal(r, outward_normal);
    rec.pMat = pMat;
    return true;
}

inline bool movingsphere::boundingBox(float t0, float t1, aabb& outBox) const
{
    glm::vec3 center = getCenter(t0);
    aabb box0(center - glm::vec3(radius), center + glm::vec3(radius));
    center = getCenter(t1);
    aabb box1(center - glm::vec3(radius), center + glm::vec3(radius));
    outBox = surrounding_box(box0, box1);
    return true;
}

class XYRect: public hittable
{
public:
    XYRect(float _x0, float _x1, float _y0, float _y1, float _z, shared_ptr<material> _mat);
    bool boundingBox(float t0, float t1, aabb& outBox) const override;
    bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
protected:
    float x0, x1, y0, y1, k;
    shared_ptr<material> pMat;
};

inline XYRect::XYRect(float _x0, float _x1, float _y0, float _y1, float _z, shared_ptr<material> _mat):
		x0(_x0), x1(_x1), y0(_y0), y1(_y1), k(_z), pMat(std::move(_mat)) {}

inline bool XYRect::boundingBox(float t0, float t1, aabb& outBox) const
{
    outBox = aabb(glm::vec3(x0, y0, k - 0.001f), glm::vec3(x1, y1, k + 0.001f));
    return true;
}

inline bool XYRect::hit(const ray& r, double t_min, double t_max, hit_record& rec) const
{
    float t = (k - r.origin().z) / r.direction().z;
    if (t < t_min || t > t_max) { return false; }
    float x = r.origin().x + t * r.direction().x;
    float y = r.origin().y + t * r.direction().y;
    if (x < x0 || x > x1 || y < y0 || y > y1) { return false; }
    auto outward_normal = glm::vec3(0, 0, 1);
    rec.set_face_normal(r, outward_normal);
    rec.pMat = pMat;
    rec.t = t;
    rec.u = (x - x0) / (x1 - x0);
    rec.v = (y - y0) / (y1 - y0);
    rec.p = r.at(t);
}

class YZRect :public hittable
{
public:
    YZRect(float _y0, float _y1, float _z0, float _z1, float _x, shared_ptr<material> _mat);
    bool boundingBox(float t0, float t1, aabb& outBox) const override;
    bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
protected:
    float y0, y1, z0, z1, k;
    shared_ptr<material> pMat;
};

inline YZRect::YZRect(float _y0, float _y1, float _z0, float _z1, float _x, shared_ptr<material> _mat) :
	y0(_y0), y1(_y1), z0(_z0), z1(_z1), k(_x), pMat(std::move(_mat)) {}

inline bool YZRect::boundingBox(float t0, float t1, aabb& outBox) const
{
    outBox = aabb(glm::vec3(k - 0.001f, y0, z0), glm::vec3(k + 0.001f, y1, z1));
    return true;
}

inline bool YZRect::hit(const ray& r, double t_min, double t_max, hit_record& rec) const
{
    auto t = (k - r.origin().x) / r.direction().x;
    if (t < t_min || t > t_max)
        return false;
    auto y = r.origin().y + t * r.direction().y;
    auto z = r.origin().z + t * r.direction().z;
    if (y < y0 || y > y1 || z < z0 || z > z1)
        return false;
    rec.u = (y - y0) / (y1 - y0);
    rec.v = (z - z0) / (z1 - z0);
    rec.t = t;
    auto outward_normal = glm::vec3(1, 0, 0);
    rec.set_face_normal(r, outward_normal);
    rec.pMat = pMat;
    rec.p = r.at(t);
    return true;
}

class XZRect : public hittable
{
public:
    XZRect(float _x0, float _x1, float _z0, float _z1, float _y, shared_ptr<material> _mat);
    bool boundingBox(float t0, float t1, aabb& outBox) const override;
    bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
protected:
    float x0, x1, z0, z1, k;
    shared_ptr<material> pMat;
};

inline XZRect::XZRect(float _x0, float _x1, float _z0, float _z1, float _y, shared_ptr<material> _mat) :
    x0(_x0), x1(_x1), z0(_z0), z1(_z1), k(_y), pMat(std::move(_mat)) {}

inline bool XZRect::boundingBox(float t0, float t1, aabb& outBox) const
{
    outBox = aabb(glm::vec3(x0, k - 0.001f, z0), glm::vec3(x1, k + 0.001f, z1));
    return true;
}

inline bool XZRect::hit(const ray& r, double t_min, double t_max, hit_record& rec) const
{
    float t = (k - r.origin().y) / r.direction().y;
    if (t < t_min || t > t_max) { return false; }
    float x = r.origin().x + t * r.direction().x;
    float z = r.origin().z + t * r.direction().z;
    if (x < x0 || x > x1 || z < z0 || z > z1) { return false; }
    auto outward_normal = glm::vec3(0, 1, 0);
    rec.set_face_normal(r, outward_normal);
    rec.pMat = pMat;
    rec.t = t;
    rec.u = (x - x0) / (x1 - x0);
    rec.v = (z - z0) / (z1 - z0);
    rec.p = r.at(t);
}

// helper functions
aabb surrounding_box(aabb box0, aabb box1) {
    glm::vec3 small(fmin(box0.min().x, box1.min().x),
        fmin(box0.min().y, box1.min().y),
        fmin(box0.min().z, box1.min().z));

    glm::vec3 big(fmax(box0.max().x, box1.max().x),
        fmax(box0.max().y, box1.max().y),
        fmax(box0.max().z, box1.max().z));

    return aabb(small, big);
}

inline bool box_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b, int axis) {
    aabb box_a;
    aabb box_b;

    if (!a->boundingBox(0, 0, box_a) || !b->boundingBox(0, 0, box_b))
        std::cerr << "No bounding box in bvh_node constructor.\n";

    return box_a.min()[axis] < box_b.min()[axis];
}

bool box_x_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
    return box_compare(a, b, 0);
}

bool box_y_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
    return box_compare(a, b, 1);
}

bool box_z_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
    return box_compare(a, b, 2);
}
