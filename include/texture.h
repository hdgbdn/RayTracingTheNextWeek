#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "rtnextweek.h"
#include <memory>
#include "perlin.h"

class texture
{
public:
	virtual vec3 value(float u, float v, const vec3& p) const = 0;
};

class solid_color : public texture
{
public:
	solid_color(): color_value() {}
	solid_color(const vec3& c) : color_value(c) {}
	solid_color(float r, float g, float b) : solid_color(vec3(r, g, b)) {}
	vec3 value(float u, float v, const vec3& p) const override { return color_value; };
private:
	vec3 color_value;
};

class checker_texture: public texture
{
public:
    checker_texture() {}

    checker_texture(shared_ptr<texture> _even, shared_ptr<texture> _odd)
        : even(_even), odd(_odd) {}

    checker_texture(glm::vec3 c1, glm::vec3 c2)
        : even(make_shared<solid_color>(c1)), odd(make_shared<solid_color>(c2)) {}

    virtual vec3 value(float u, float v, const vec3& p) const override {
        auto sines = sin(10 * p.x) * sin(10 * p.y) * sin(10 * p.z);
        if (sines < 0)
            return odd->value(u, v, p);
        else
            return even->value(u, v, p);
    }

public:
    shared_ptr<texture> odd;
    shared_ptr<texture> even;
};

class NoiseTexture :public texture
{
public:
    NoiseTexture(): noise() {}
    vec3 value(float u, float v, const vec3& p) const override
    {
        return vec3(1, 1, 1) * noise.noise(p);
    }
private:
    perlin noise;
};

#endif // !TEXTURE_H_
