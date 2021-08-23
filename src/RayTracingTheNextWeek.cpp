﻿// RayTracingTheNextWeek.cpp : Defines the entry point for the application.
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
#include "aabb.h"
using namespace std;
using namespace hdgbdn;

// configs
const string APP_NAME = "Ray Tracing The Next Week";
const int window_width = 400;
const int window_height = 400;
const double infinity = std::numeric_limits<double>::infinity();
const int samples = 2;
const int ray_depth = 5;

const float aspect_ratio = static_cast<float>(window_width) / window_height;

hittable_list random_scene() {
	hittable_list world;

	auto ground_material = make_shared<lambertian>(vec3(0.5, 0.5, 0.5));
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

int main()
{
	Window win(window_width, window_height, APP_NAME);
	Shader shader("res/shaders/base.vs", "res/shaders/base.fs");
	hittable_list world = random_scene();
	FullScreenQuad screenBuffer;
	glm::vec3 eye(13, 2, 3);
	glm::vec3 center(0, 0, 0);
	glm::vec3 up(0.f, 1.f, 0.f);
	blurcamera cam(eye, center, up, 10, 2, 2 * aspect_ratio, 0.1, 0.f, 1.f);
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	auto* data = new unsigned char[window_height * window_width * 3];
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window_width, window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

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

	std::function<glm::vec3(const ray&, const hittable_list&, int)> ray_color = [&](const ray& r, const hittable_list& list, int depth)->glm::vec3
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
						color += ray_color(cam.getRayFromScreenPos(u + rtnextweek::random_double() / (window_height - 1), v + rtnextweek::random_double() / (window_width - 1)), world, ray_depth);
					}
					color /= samples;
					setPixelColor(j, i, data, color);
				}
			}
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window_width, window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			screenBuffer.Draw(shader, texture);
			glfwPollEvents();
			glfwSwapBuffers(win.get());
	});
	Window::StartRenderLoop(win);
	return 0;
}