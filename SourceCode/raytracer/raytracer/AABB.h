#pragma once
#include <string>
#include "CRTTypes.h"

class AABB {
    AABB(const Vec3& min, const Vec3& max) : min(min), max(max) {}

    bool intersects(const AABB& other) const;

    bool intersect(const Ray& ray) const;

    bool contains(const Vec3& point) const;

    void expand(const Vec3& point);

    Vec3 getCenter() const;

    Vec3 getSize() const;

    std::string toString() const;

private:
    Vec3 min, max;
};
