// RayTracingTheNextWeek.cpp : Defines the entry point for the application.
//
#include <iostream>
#include <string>
#include "glad/glad.h"
#include "glm/glm.hpp"
#include "Window.h"
#include "Shader.h"
#include "shapes.h"
#include "camera.h"
#include "ray.h"
#include "hittable.h"
#include "material.h"
#include "bvh.h"
#include "texture.h"
#include "perlin.h"
using namespace std;
using namespace hdgbdn;

// configs
const string APP_NAME = "Ray Tracing The Next Week";
const int window_width = 300;
const int window_height = 300;
const double infinity = std::numeric_limits<double>::infinity();
const int samples = 20;
const int ray_depth = 10;

const float aspect_ratio = static_cast<float>(window_width) / window_height;

GLuint createTexture()
{
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	return texture;
}

void sendTexture(void* data)
{
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window_width, window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
}

hittable_list random_scene() {
	hittable_list world;

	auto checker_tex = make_shared<checker_texture>(vec3(0.2, 0.3, 0.1), vec3(0.9, 0.9, 0.9));
	auto ground_material = make_shared<lambertian>(checker_tex);
	world.add(make_shared<sphere>(vec3(0, -1000, 0), 1000, ground_material));

	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {
			auto choose_mat = rtnextweek::random_double();
			vec3 center(a + 0.9 * rtnextweek::random_double(), 0.2, b + 0.9 * rtnextweek::random_double());

			if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
				shared_ptr<material> sphere_material;

				if (choose_mat < 0.8) {
					// diffuse
					auto albedo = vec3(rtnextweek::random_double(), rtnextweek::random_double(), rtnextweek::random_double());
					sphere_material = make_shared<lambertian>(albedo);
					glm::vec3 center2 = center + vec3(0, rtnextweek::random_double(0, .5), 0);
					world.add(make_shared<movingsphere>(center, center2, 0.f, 1.f, 0.2, sphere_material));
				}
				else if (choose_mat < 0.95) {
					// metal
					auto albedo = vec3(rtnextweek::random_double(0.5, 1.0), rtnextweek::random_double(0.5, 1.0), rtnextweek::random_double(0.5, 1.0));
					auto fuzz = rtnextweek::random_double(0, 0.5);
					sphere_material = make_shared<FuzzyMetal>(albedo, fuzz);
					world.add(make_shared<sphere>(center, 0.2, sphere_material));
				}
				else {
					// glass
					sphere_material = make_shared<dielectric>(1.5);
					world.add(make_shared<sphere>(center, 0.2, sphere_material));
				}
			}
		}
	}

	auto material1 = make_shared<dielectric>(1.5);
	world.add(make_shared<sphere>(vec3(0, 1, 0), 1.0, material1));

	auto material2 = make_shared<lambertian>(vec3(0.4, 0.2, 0.1));
	world.add(make_shared<sphere>(vec3(-4, 1, 0), 1.0, material2));

	auto material3 = make_shared<metal>(vec3(0.7, 0.6, 0.5));
	world.add(make_shared<sphere>(vec3(4, 1, 0), 1.0, material3));

	return world;
}

hittable_list twoSphere()
{
	auto noiseTexture = make_shared<NoiseTexture>(2);
	auto noiseMat = make_shared<lambertian>(noiseTexture);
	hittable_list world;
	world.add(make_shared<sphere>(vec3(0, 0, 0), 5.0, noiseMat));
	world.add(make_shared<sphere>(vec3(0, -1000, 0), 995.0, noiseMat));
	return world;
}

hittable_list planet()
{
	auto planetTexture = make_shared<ImageTexture>("res/textures/Gaseous4.png");
	auto planetTexture2 = make_shared<ImageTexture>("res/textures/moonmap4k.jpg");
	auto planetMat = make_shared<lambertian>(planetTexture);
	auto planetMat2 = make_shared<lambertian>(planetTexture2);
	hittable_list world;
	world.add(make_shared<sphere>(vec3(0, 0, 0), 5.0, planetMat));
	world.add(make_shared<sphere>(vec3(0, -30, 0), 25, planetMat2));
	return world;
}

int main()
{
	Window win(window_width, window_height, APP_NAME);
	Shader shader("res/shaders/base.vs", "res/shaders/base.fs");
	FullScreenQuad screenBuffer;
	shared_ptr<camera> cam;
	shared_ptr<BVHnode> world;
	
	switch (1)
	{
		glm::vec3 eye;
		glm::vec3 center;
		glm::vec3 up;
	default:
	case 0:
		eye = vec3(5, 2, 8);
		center = vec3(0, 0, 0);
		up = vec3(0.f, 1.f, 0.f);
		world = make_shared<BVHnode>(random_scene(), 0.f, 1.f);
		cam = make_shared<blurcamera>(eye, center, up, 1, 2, 2 * aspect_ratio, 0.1, 0.f, 1.f);
		break;
	case 1:
		eye = vec3(5, 2, 8);
		center = vec3(0, 0, 0);
		up = vec3(0.f, 1.f, 0.f);
		world = make_shared<BVHnode>(twoSphere(), 0.f, 1.f);
		cam = make_shared<camera>(eye, center, up, 1, 2, 2 * aspect_ratio, 0.f, 1.f);
	case 2:
		eye = vec3(0, 20, 100);
		center = vec3(0, 0, 0);
		up = vec3(0.f, 1.f, 0.f);
		world = make_shared<BVHnode>(planet(), 0.f, 1.f);
		cam = make_shared<camera>(eye, center, up, 10, 2, 2 * aspect_ratio, 0.f, 1.f);
	}
	
	GLuint texture = createTexture();

	auto* data = new unsigned char[window_height * window_width * 3];

	auto setPixelColor = [](int h, int w, unsigned char* p, const glm::vec3& col)
	{
		int index = 3 * (h * window_width + w);
		unsigned char r = sqrt(col.r) * 255;
		unsigned char g = sqrt(col.g) * 255;
		unsigned char b = sqrt(col.b) * 255;
		p[index++] = r;
		p[index++] = g;
		p[index++] = b;
	};

	std::function<glm::vec3(const ray&, const hittable&, int)> ray_color = [&](const ray& r, const hittable& list, int depth)->glm::vec3
	{
		hit_record record;
		if (depth <= 0) return vec3(0.f);
		if (list.hit(r, .001, infinity, record))
		{
			ray scattered;
			vec3 attenuation;
			if (record.pMat->scatter(r, record, attenuation, scattered))
			{
				return attenuation * ray_color(scattered, list, depth - 1);
			}
			else
			{
				return vec3(0);
			}
		}
		glm::vec3 normDir = glm::normalize(r.direction());
		float t = 0.5 * (normDir.y + 1);
		return t * glm::vec3(0.5, 0.7, 1.0) + (1 - t) * glm::vec3(1);
	};

	win.SetRenderOperation([&]()
	{
			glDisable(GL_DEPTH_TEST);
			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			for (int j = window_height - 1; j >= 0; --j) {
				for (int i = 0; i < window_width; ++i) {
					float u = static_cast<float>(j) / window_height;
					float v = static_cast<float>(i) / window_width;
					glm::vec3 color(0.f);
					for (int s = 0; s < samples; ++s)
					{
						color += ray_color(cam->getRayFromScreenPos(u + rtnextweek::random_double() / (window_height - 1), v + rtnextweek::random_double() / (window_width - 1)), *world, ray_depth);
					}
					color /= samples;
					setPixelColor(j, i, data, color);
				}
			}
			sendTexture(data);
			screenBuffer.Draw(shader, texture);
			glfwPollEvents();
			glfwSwapBuffers(win.get());
	});
	Window::StartRenderLoop(win);
	return 0;
}