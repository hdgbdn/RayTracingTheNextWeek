#ifndef PERLIN_H_
#define PERLIN_H_

#include "rtnextweek.h"

class perlin
{
public:
	perlin()
	{
		ranvec = new vec3[pointCount];
		for (int i = 0; i < pointCount; ++i) {
			ranvec[i] = rtnextweek::random_unit_vector();
		}
		permX = perlinGeneratePerm();
		permY = perlinGeneratePerm();
		permZ = perlinGeneratePerm();
	}

	float noise(const glm::vec3& p) const
	{
		auto u = p.x - floor(p.x);
		auto v = p.y - floor(p.y);
		auto w = p.z - floor(p.z);

		auto i = static_cast<int>(floor(p.x));
		auto j = static_cast<int>(floor(p.y));
		auto k = static_cast<int>(floor(p.z));
		vec3 c[2][2][2];

		for (int di = 0; di < 2; di++)
			for (int dj = 0; dj < 2; dj++)
				for (int dk = 0; dk < 2; dk++)
					c[di][dj][dk] = ranvec[
						permX[(i + di) & 255] ^
							permY[(j + dj) & 255] ^
							permZ[(k + dk) & 255]
					];

		return perlinInterp(c, u, v, w);
	}

	float turb(const vec3& p, int depth = 7) const {
		auto accum = 0.0;
		auto temp_p = p;
		auto weight = 1.0;

		for (int i = 0; i < depth; i++) {
			accum += weight * noise(temp_p);
			weight *= 0.5;
			temp_p *= 2;
		}

		return fabs(accum);
	}
	~perlin()
	{
		delete[] ranvec;
		delete[] permX;
		delete[] permY;
		delete[] permZ;
	}
private:
	static const int pointCount = 256;
	vec3* ranvec;
	int* permX;
	int* permY;
	int* permZ;
	static int* perlinGeneratePerm()
	{
		auto p = new int[pointCount];
		for (int i = 0; i < pointCount; ++i)
			p[i] = i;
		permute(p, pointCount);
		return p;
	}
	static void permute(int* p, int n)
	{
		for(int i = n-1; i >0; --i)
		{
			int  target = rtnextweek::random_int(0, i);
			int tmp = p[i];
			p[i] = p[target];
			p[target] = tmp;
		}
	}
	static float perlinInterp(vec3 c[2][2][2], float u, float v, float w) {
		auto uu = u * u * (3 - 2 * u);
		auto vv = v * v * (3 - 2 * v);
		auto ww = w * w * (3 - 2 * w);
		auto accum = 0.0;

		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++) {
					vec3 weight_v(u - i, v - j, w - k);
					accum += (i * uu + (1 - i) * (1 - uu))
						* (j * vv + (1 - j) * (1 - vv))
						* (k * ww + (1 - k) * (1 - ww))
						* glm::dot(c[i][j][k], weight_v);
				}

		return accum;
	}
};

#endif