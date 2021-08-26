#ifndef MATERIAL_H_
#define MATERIAL_H_

#include "hittable.h"
#include "glm/glm.hpp"
#include "texture.h"

using namespace glm;

class material
{
public:
	virtual bool scatter(
		const ray& rIn, const hit_record& record, vec3& attenuation, ray& scattered
	) const = 0;
	virtual vec3 emitted(float, float, const vec3&) const
	{
		return vec3(0, 0, 0);
	}
};

class lambertian: public material
{
public:
	lambertian(const vec3&);
	lambertian(shared_ptr<texture>);
	virtual bool scatter(const ray& rIn, const hit_record& rec, vec3& attenuation, ray& scattered) const override;
private:
	shared_ptr<texture> albeo;
};

inline lambertian::lambertian(const vec3& color): material(), albeo(make_shared<solid_color>(color)) {}

inline lambertian::lambertian(shared_ptr<texture> t): material(), albeo(t) {}

inline bool lambertian::scatter(const ray& rIn, const hit_record& record, vec3& attenuation, ray& scattered) const
{
	vec3 scatteredDirection = rtnextweek::random_in_hemisphere(record.normal);
	scattered = ray(record.p, scatteredDirection, rIn.time());
	attenuation = albeo->value(record.u, record.v, record.p);
	return true;
}

class metal: public material
{
public:
	metal(const vec3&);
	virtual bool scatter(const ray& rIn, const hit_record& record, vec3& attenuation, ray& scattered) const override;
protected:
	vec3 albeo;
};

inline metal::metal(const vec3& color) :albeo(color) {}


inline bool metal::scatter(const ray& rIn, const hit_record& record, vec3& attenuation, ray& scattered) const
{
	vec3 scatteredDirection = rtnextweek::reflect(glm::normalize(rIn.direction()), record.normal);
	scattered = ray(record.p, scatteredDirection, rIn.time());
	attenuation = albeo;
	return true;
}

class FuzzyMetal: public metal
{
public:
	FuzzyMetal(const vec3&, double);
	virtual bool scatter(const ray& rIn, const hit_record& record, vec3& attenuation, ray& scattered) const override;
protected:
	double fuzzy;
};

inline FuzzyMetal::FuzzyMetal(const vec3& color, double f): metal(color), fuzzy(f) {}

inline bool FuzzyMetal::scatter(const ray& rIn, const hit_record& record, vec3& attenuation, ray& scattered) const
{
	vec3 scatteredDirection = rtnextweek::reflect(glm::normalize(rIn.direction()), record.normal);
	scattered = ray(record.p, scatteredDirection + static_cast<float>(fuzzy) * rtnextweek::random_in_hemisphere(scatteredDirection), rIn.time());
	attenuation = albeo;
	return true;
}

class dielectric : public material {
public:
	dielectric(double index_of_refraction) : material(), ir(index_of_refraction) {}

	virtual bool scatter(
		const ray& rIn, const hit_record& record, vec3& attenuation, ray& scattered
	) const override {
		attenuation = vec3(1.0, 1.0, 1.0);
		float refraction_ratio = record.front_face ? (1.0 / ir) : ir;
		vec3 unit_direction = normalize(rIn.direction());
		float cos_theta = fmin(dot(-unit_direction, record.normal), 1.0);
		float sin_theta = sqrt(1.0 - cos_theta * cos_theta);
		bool cannot_refract = refraction_ratio * sin_theta > 1.0;
		vec3 direction;
		if (cannot_refract) { direction = rtnextweek::reflect(unit_direction, record.normal); }
		else { direction = rtnextweek::refract(unit_direction, record.normal, refraction_ratio); }
		scattered = ray(record.p, direction, rIn.time());
		return true;
	}

protected:
	double ir; // Index of Refraction
};

class DiffuseLight :public material
{
public:
	DiffuseLight(shared_ptr<texture> t) : emit(t) {}
	DiffuseLight(const vec3& c) : emit(make_shared<solid_color>(c)) {}
	virtual bool scatter(const ray& rIn, const hit_record& record, vec3& attenuation, ray& scattered) const override
	{
		return false;
	}
	virtual vec3 emitted(float u, float v, const vec3& p) const
	{
		return emit->value(u, v, p);
	}
protected:
	shared_ptr<texture> emit;
};

#endif
