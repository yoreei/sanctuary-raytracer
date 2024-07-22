#include "AABB.h"

#include <iostream>
#include <algorithm>
#include <string>
#include <sstream>

bool AABB::intersects(const AABB& other) const {
    return (min.x <= other.max.x && max.x >= other.min.x) &&
        (min.y <= other.max.y && max.y >= other.min.y) &&
        (min.z <= other.max.z && max.z >= other.min.z);
}

bool AABB::intersect(const Ray& ray) const {
    float tmin = (min.x - ray.origin.x) / ray.direction.x;
    float tmax = (max.x - ray.origin.x) / ray.direction.x;

    if (tmin > tmax) std::swap(tmin, tmax);

    float tymin = (min.y - ray.origin.y) / ray.direction.y;
    float tymax = (max.y - ray.origin.y) / ray.direction.y;

    if (tymin > tymax) std::swap(tymin, tymax);

    if ((tmin > tymax) || (tymin > tmax))
        return false;

    if (tymin > tmin)
        tmin = tymin;

    if (tymax < tmax)
        tmax = tymax;

    float tzmin = (min.z - ray.origin.z) / ray.direction.z;
    float tzmax = (max.z - ray.origin.z) / ray.direction.z;

    if (tzmin > tzmax) std::swap(tzmin, tzmax);

    if ((tmin > tzmax) || (tzmin > tmax))
        return false;

    return true;
}

bool AABB::contains(const Vec3& point) const {
    return (point.x >= min.x && point.x <= max.x) &&
        (point.y >= min.y && point.y <= max.y) &&
        (point.z >= min.z && point.z <= max.z);
}

void AABB::expand(const Vec3& point) {
    min.x = std::min(min.x, point.x);
    min.y = std::min(min.y, point.y);
    min.z = std::min(min.z, point.z);

    max.x = std::max(max.x, point.x);
    max.y = std::max(max.y, point.y);
    max.z = std::max(max.z, point.z);
}

Vec3 AABB::getCenter() const {
    return Vec3((min.x + max.x) / 2, (min.y + max.y) / 2, (min.z + max.z) / 2);
}

Vec3 AABB::getSize() const {
    return max - min;
}

inline std::string AABB::toString() const {
    std::stringstream ss;
    ss << "Min: (" << min.x << ", " << min.y << ", " << min.z << ")\n";
    ss << "Max: (" << max.x << ", " << max.y << ", " << max.z << ")\n";
    return ss.str();
}
