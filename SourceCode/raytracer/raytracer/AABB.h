#pragma once
#include <string>
#include "CRTTypes.h"

class TraceHit;
class Scene;
class Ray;
class Triangle;
class AABB {
public:
    AABB(const Vec3& min, const Vec3& max) : bounds{min, max} {}

    bool intersects(const AABB& other) const;

    void intersect(const Scene& scene, const Ray& ray, TraceHit& out) const;

    bool check(const Ray& ray) const;

    bool contains(const Vec3& point) const;

    void expandWithTriangle(const Scene& scene, const Triangle& triangle, const size_t triangleRef);

    Vec3 getCenter() const;

    Vec3 getSize() const;

    std::string toString() const;

    Vec3 bounds[2] {Vec3{0.f, 0.f, 0.f}, Vec3{0.f, 0.f, 0.f}};
private:
    std::vector<size_t> triangleRefs {};

    void expandWithPoint(const Vec3& point);
};
