#ifndef PERLIN_H_
#define PERLIN_H_

#include "rtnextweek.h"

class perlin
{
public:
	perlin()
	{
		randfloat = new float[pointCount];
		for(int i = 0; i < pointCount; ++i)
		{
			randfloat[i] = static_cast<float>(rtnextweek::random_double());
		}
		permX = perlinGeneratePerm();
		permY = perlinGeneratePerm();
		permZ = perlinGeneratePerm();
	}

	float noise(const glm::vec3& p) const
	{
		auto i = static_cast<int>(4 * p.x) & 255;
		auto j = static_cast<int>(4 * p.y) & 255;
		auto k = static_cast<int>(4 * p.z) & 255;

		return randfloat[permX[i] ^ permY[j] ^ permZ[k]];
	}
	~perlin()
	{
		delete[] randfloat;
		delete[] permX;
		delete[] permY;
		delete[] permZ;
	}
private:
	static const int pointCount = 256;
	float* randfloat;
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
};

#endif