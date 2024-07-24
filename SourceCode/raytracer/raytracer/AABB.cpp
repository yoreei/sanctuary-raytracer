#include "AABB.h"

#include <iostream>
#include <algorithm>
#include <string>
#include <sstream>

#include "Triangle.h"
#include "Scene.h"
#include "Metrics.h"
#include "TraceHit.h"

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

void AABB::traverse(const Scene& scene, const Ray& ray, TraceHit& out) const {
    out.t = std::numeric_limits<float>::max();
    out.type = TraceHitType::OUT_OF_BOUNDS;
    TraceHit hit {};

    if (isLeaf()) {
        for (const size_t& triRef : triangleRefs) {
            const Triangle& tri = scene.triangles[triRef];
            if (!scene.triangleAABBs[triRef].hasIntersection(ray)) continue;

            tri.intersect(scene, ray, hit);
            if (hit.successful() && hit.t < out.t) {
                out = hit;
            }
        }
    }
    else {
        child[0]->hasIntersection(ray) ? child[0]->traverse(scene, ray, out) : child[1]->traverse(scene, ray, out);
    }
}


bool AABB::contains(const Vec3& point) const {
    return (point.x >= bounds[0].x && point.x <= bounds[1].x) &&
        (point.y >= bounds[0].y && point.y <= bounds[1].y) &&
        (point.z >= bounds[0].z && point.z <= bounds[1].z);
}

void AABB::expandWithTriangle(const Scene& scene, const Triangle& triangle, const size_t triangleRef)
{
    for (size_t i = 0; i < 3; ++i) {
        const Vec3& vertex = scene.vertices[triangle.v[i]];
        this->expandWithPoint(vertex);
        triangleRefs.push_back(triangleRef);
    }
}

void AABB::expandWithPoint(const Vec3& point)
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

