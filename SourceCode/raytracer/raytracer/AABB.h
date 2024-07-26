#pragma once
#include <string>
#include <array>
#include <memory>

#include "CRTTypes.h"

class Scene;
class Ray;
class Triangle;
class AABB {
public:
    AABB() = default;

    AABB(const Vec3& min, const Vec3& max) : bounds{min, max} {}

    static AABB MakeEnclosingAABB(std::vector<AABB> aabbs);

    bool hasIntersection(const AABB& other) const;

    bool hasIntersection(const Ray& ray) const;

    float distanceToAxis(size_t axis, const Vec3& point) const;

    size_t getMaxAxis() const;

    float volume() const;

    bool contains(const Vec3& point) const;

    void expand(const Vec3& point);

    Vec3 getCenter() const;

    Vec3 getSize() const;

    std::string toString() const;

    Vec3 bounds[2] {Vec3{0.f, 0.f, 0.f}, Vec3{0.f, 0.f, 0.f}};
};
