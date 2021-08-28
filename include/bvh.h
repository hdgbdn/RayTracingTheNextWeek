#ifndef BVH_H_
#define BVH_H_

#include "hittable.h"
#include "aabb.h"
#include <memory>
#include <algorithm>


class BVHnode :public hittable
{
public:
	BVHnode(const hittable_list& list, float t0, float t1): BVHnode(list.getObjects(), 0, list.size(), t0, t1) {}
	BVHnode(
		const std::vector<shared_ptr<hittable>>& src_objects,
		size_t start, size_t end, double time0, double time1);
	bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
	bool boundingBox(float t0, float t1, aabb& outBox) const override;
protected:
	std::shared_ptr<hittable> left;
	std::shared_ptr<hittable> right;
	aabb box;
};

inline bool BVHnode::hit(const ray& r, double t_min, double t_max, hit_record& rec) const
{
	if (!box.hit(r, t_min, t_max)) return false;
	bool hit_left = left->hit(r, t_min, t_max, rec);
	bool hit_right = right->hit(r, t_min, hit_left ? rec.t : t_max, rec);

	return hit_left || hit_right;
}

inline BVHnode::BVHnode(const std::vector<shared_ptr<hittable>>& src_objects, size_t start, size_t end, double time0, double time1)
{
	auto objects = src_objects;
	int axis = rtnextweek::random_int(0, 2);
	auto comparator = (axis == 0) ? box_x_compare
		: (axis == 1) ? box_y_compare
		: box_z_compare;
	size_t span = end - start;
	if (span == 1) {
		left = right = objects[start];
	}
	else if(span == 2)
	{
		if (comparator(objects[start], objects[start + 1]))
		{
			left = objects[start];
			right = objects[start + 1];
		}
		else
		{
			right = objects[start];
			left = objects[start + 1];
		}
	}
	else
	{
		std::sort(objects.begin() + start, objects.begin() + end, comparator);
		size_t mid = start + span / 2;
		left = make_shared<BVHnode>(objects, start, mid, time0, time1);
		right = make_shared<BVHnode>(objects, mid, end, time0, time1);
	}

	aabb boxL, boxR;
	if(!left->boundingBox(time0, time1, boxL) || !right->boundingBox(time0, time1, boxR))
	{
		std::cerr << "No bounding box in bvh_node constructor.\n";
	}
	box = surrounding_box(boxL, boxR);
}

inline bool BVHnode::boundingBox(float t0, float t1, aabb& outBox) const
{
	outBox = box;
	return true;
}


#endif