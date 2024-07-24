#include "AABB.h"

#include <iostream>
#include <algorithm>
#include <string>
#include <sstream>

#include "Triangle.h"
#include "Scene.h"
#include "Metrics.h"
#include "TraceHit.h"

AABB AABB::MakeEnclosingAABB(std::vector<AABB> aabbs)
{
    Vec3 min = Vec3::MakeMax();
    Vec3 max = Vec3::MakeLowest();

    for (const auto& aabb : aabbs)
    {
        min.x = std::min(min.x, aabb.bounds[0].x);
        min.y = std::min(min.y, aabb.bounds[0].y);
        min.z = std::min(min.z, aabb.bounds[0].z);

        max.x = std::max(max.x, aabb.bounds[1].x);
        max.y = std::max(max.y, aabb.bounds[1].y);
        max.z = std::max(max.z, aabb.bounds[1].z);
    }

    return {min, max};
}

bool AABB::hasIntersection(const AABB& other) const {
    return (bounds[0].x <= other.bounds[1].x && bounds[1].x >= other.bounds[0].x) &&
        (bounds[0].y <= other.bounds[1].y && bounds[1].y >= other.bounds[0].y) &&
        (bounds[0].z <= other.bounds[1].z && bounds[1].z >= other.bounds[0].z);
}

bool AABB::hasIntersection(const Ray& r) const {
    float tmin, tmax, tymin, tymax, tzmin, tzmax;
    
    tmin = (bounds[r.sign[0]].x - r.origin.x) * r.invdir.x;
    tmax = (bounds[1-r.sign[0]].x - r.origin.x) * r.invdir.x;
    tymin = (bounds[r.sign[1]].y - r.origin.y) * r.invdir.y;
    tymax = (bounds[1-r.sign[1]].y - r.origin.y) * r.invdir.y;
    
    if ((tmin > tymax) || (tymin > tmax))
        return false;

    if (tymin > tmin)
        tmin = tymin;
    if (tymax < tmax)
        tmax = tymax;
    
    tzmin = (bounds[r.sign[2]].z - r.origin.z) * r.invdir.z;
    tzmax = (bounds[1-r.sign[2]].z - r.origin.z) * r.invdir.z;
    
    if ((tmin > tzmax) || (tzmin > tmax))
        return false;

    if (tzmin > tmin)
        tmin = tzmin;
    if (tzmax < tmax)
        tmax = tzmax;

    return true;
}

size_t AABB::getMaxAxis() const
{
    Vec3 extents = bounds[1] - bounds[0];
    if (extents.x > extents.y && extents.x > extents.z) {
        return 0; // x-axis
    } else if (extents.y > extents.z) {
        return 1; // y-axis
    } else {
        return 2; // z-axis
    }
}



bool AABB::contains(const Vec3& point) const {
    return (point.x >= bounds[0].x && point.x <= bounds[1].x) &&
        (point.y >= bounds[0].y && point.y <= bounds[1].y) &&
        (point.z >= bounds[0].z && point.z <= bounds[1].z);
}

void AABB::expand(const Vec3& point)
{
    bounds[0].x = std::min(bounds[0].x, point.x);
    bounds[0].y = std::min(bounds[0].y, point.y);
    bounds[0].z = std::min(bounds[0].z, point.z);

    bounds[1].x = std::max(bounds[1].x, point.x);
    bounds[1].y = std::max(bounds[1].y, point.y);
    bounds[1].z = std::max(bounds[1].z, point.z);
}


Vec3 AABB::getCenter() const {
    return Vec3((bounds[0].x + bounds[1].x) / 2, (bounds[0].y + bounds[1].y) / 2, (bounds[0].z + bounds[1].z) / 2);
}

Vec3 AABB::getSize() const {
    return bounds[1] - bounds[0];
}

inline std::string AABB::toString() const {
    std::stringstream ss;
    ss << "bounds[0]: (" << bounds[0].x << ", " << bounds[0].y << ", " << bounds[0].z << ")\n";
    ss << "bounds[1]: (" << bounds[1].x << ", " << bounds[1].y << ", " << bounds[1].z << ")\n";
    return ss.str();
}

float AABB::volume() const {
    float width = bounds[1].x - bounds[0].x;
    float height = bounds[1].y - bounds[0].y;
    float depth = bounds[1].z - bounds[0].z;

    return width * height * depth;
}
