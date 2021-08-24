#pragma once
#include "rtnextweek.h"
#include "glm/glm.hpp"

class aabb {
public:
    aabb() {}
    aabb(const glm::vec3& min, const glm::vec3& max) { minimum = min; maximum = max; }

    glm::vec3 min() const { return minimum; }
    glm::vec3 max() const { return maximum; }

    bool hit(const ray& r, double t_min, double t_max) const {
        for (int a = 0; a < 3; a++) {
            auto invD = 1.0f / r.direction()[a];
            auto t0 = (min()[a] - r.origin()[a]) * invD;
            auto t1 = (max()[a] - r.origin()[a]) * invD;
            if (invD < 0.0f)
                std::swap(t0, t1);
            t_min = t0 > t_min ? t0 : t_min;
            t_max = t1 < t_max ? t1 : t_max;
            if (t_max <= t_min)
                return false;
        }
        return true;
    }

    glm::vec3 minimum;
    glm::vec3 maximum;
};