#pragma once

#include "hittable.h"
#include "material.h"
#include "texture.h"

class ConstantMedium : public hittable
{
public:
	ConstantMedium(shared_ptr<hittable> b, float d, shared_ptr<texture> a)
		: boundary(b), phaseFunction(make_shared<Isotropic>(a)) , negInvDensity(-1/d) {}
	ConstantMedium(shared_ptr<hittable> b, float d, const vec3& c)
		: boundary(b), phaseFunction(make_shared<Isotropic>(c)), negInvDensity(-1 / d) {}

	bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
	bool boundingBox(float t0, float t1, aabb& outBox) const override;
private:
	shared_ptr<hittable> boundary;
	shared_ptr<material> phaseFunction;
	float negInvDensity;
};

inline bool ConstantMedium::hit(const ray& r, double t_min, double t_max, hit_record& rec) const
{
	const bool enableDebug = false;
	const bool debugging = enableDebug && rtnextweek::random_double() < 0.00001;

	hit_record rec1, rec2;

	float fMax = std::numeric_limits<float>::max();
	float fMin = -fMax;

	// must hit twice(hit through boundary)
	if (!boundary->hit(r, fMin, fMax, rec1)) return false;
	if (!boundary->hit(r, rec1.t + 0.0001, fMax, rec2)) return false;

	if(debugging) std::cerr << "\nt_min=" << rec1.t << ", t_max=" << rec2.t << '\n';

	if (rec1.t < t_min) rec1.t = t_min;
	if (rec2.t > t_max) rec2.t = t_max;

	if (rec1.t >= rec2.t) return false;

	if (rec1.t < 0) rec1.t = 0;

	const auto rayLength = glm::length(r.direction());
	const auto distanceInsideBoundary = (rec2.t - rec1.t) * rayLength;
	const auto hitDistance = negInvDensity * log(rtnextweek::random_double());

	if (hitDistance > distanceInsideBoundary) return false;

	rec.t = rec1.t + hitDistance / rayLength;
	rec.p = r.at(rec.t);
	if (debugging) {
		std::cerr << "hitDistance = " << hitDistance << '\n'
			<< "rec.t = " << rec.t << '\n'
			<< "rec.p = " << rec.p.x << ' ' << rec.p.y << ' ' << rec.p.z << '\n';
	}

	rec.normal = vec3(1, 0, 0);  // arbitrary
	rec.front_face = true;     // also arbitrary
	rec.pMat = phaseFunction;

	return true;
}

inline bool ConstantMedium::boundingBox(float t0, float t1, aabb& outBox) const
{
	return boundary->boundingBox(t0, t1, outBox);
}

