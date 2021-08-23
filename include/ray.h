#ifndef RAY_H
#define RAY_H

#include "glm/glm.hpp"

class ray {
public:
    ray() {}
    ray(const glm::vec3& origin, const glm::vec3& direction, float tim = 0.f)
        : orig(origin), dir(direction), tm(tim)
    {}

    glm::vec3 origin() const { return orig; }
    glm::vec3 direction() const { return dir; }
    float time() const { return tm; }

    glm::vec3 at(float t) const {
        return orig + dir * t;
    }

private:
    glm::vec3 orig;
    glm::vec3 dir;
    float tm;
};

#endif