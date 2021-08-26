#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "rtnextweek.h"
#include <memory>
#include "perlin.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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
    NoiseTexture(float s): noise(), scale(s) {}
    NoiseTexture() : NoiseTexture(1.f) {}
    vec3 value(float u, float v, const vec3& p) const override
    {
        return vec3(1, 1, 1) * 0.5f * (1 + sin(scale * p.z + 10 * noise.turb(p)));
    }
private:
    perlin noise;
    float scale;
};

class ImageTexture :public texture
{
public:
    const static int bytesPerPixel = 3;
    ImageTexture() :
        width(0), height(0), data(nullptr), bytesPerScanLine(0) {}
	ImageTexture(const char* filePath):
        width(0), height(0),
		data(stbi_load(
            filePath, &width, &height, &channels, bytesPerPixel)),
		bytesPerScanLine(bytesPerPixel * width)
    {
	    if(data == nullptr)
	    {
            std::cerr << "ERROR: Could not load texture image file '" << filePath << "'.\n";
            width = height = 0;
	    }
    }
    ~ImageTexture()
    {
        delete data;
    }
    virtual vec3 value(float u, float v, const vec3& p) const override
    {
        if (data == nullptr) return vec3(0.f, 1.f, 1.f);
        u = clamp(u, 0.f, 1.f);
        v = 1.f - clamp(v, 0.f, 1.f);

        auto i = static_cast<int>(u * width);
        auto j = static_cast<int>(v * height);

        const auto colorScale = 1.f / 255.f;
        auto pixel = data + j * bytesPerScanLine + i * bytesPerPixel;
        return vec3(colorScale * pixel[0], colorScale * pixel[1], colorScale * pixel[2]);
    }
private:
    int width;
    int height;
    unsigned char* data;
    int bytesPerScanLine;
    int channels;
};

#endif // !TEXTURE_H_