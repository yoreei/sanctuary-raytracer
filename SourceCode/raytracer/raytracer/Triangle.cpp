#include "Triangle.h"
#include "Scene.h"

/*
* @Danny search OneNote 'Triangle Intersect'
* Input: ray, rayProj
* Input: rayProj: projection of ray.direction onto camera direction
* Output: t: distance from ray.origin to the intersection point
* Output: p: intersection point
* Output: n: triangle normal (unit)
* Output: u, v: triangle-space coordinates of intersection for UV mapping
* return: true if the ray intersects the triangle
*/
// TODO return bool

Intersection Triangle::intersect(const Scene& scene, const Ray& ray, IntersectionData& out) const {
    const Vec3& v0 = scene.vertices[v[0]];
    const Vec3& v1 = scene.vertices[v[1]];
    const Vec3& v2 = scene.vertices[v[2]];

    // Here assuming counter-clockwise order
    Vec3 e0 = v1 - v0;
    Vec3 e1 = v2 - v0;

    float rayProj = ray.direction.dot(normal);

    // if triangle facing ray
    if (rayProj < -1e-6) {
        float rpDist = dot(normal, v0 - ray.origin);
        out.t = rpDist / rayProj;
        if (out.t < -1e-6) {
            return Intersection::BEHIND_RAY_ORIGIN; // Not reachable?
        }
        out.p = ray.origin + ray.direction * out.t;

        // check if `p` is inside triangle
        Vec3 v0p = out.p - v0;
        Vec3 e2 = v2 - v1;
        Vec3 c0 = cross(e0, v0p);
        Vec3 c1 = cross(v0p, e1);
        Vec3 c2 = cross(e2, out.p - v1);

        bool inside = dot(normal, c0) > 1e-6 &&
            dot(normal, c1) > 1e-6 &&
            dot(normal, c2) > 1e-6;

        if (inside) {
            float area_inv = 1.f / e0.crossLength(e1);
            out.u = c0.length() * area_inv;
            out.v = c1.length() * area_inv;
            out.materialIndex = this->materialIndex;
            out.n = intersectionNormal(scene, out.u, out.v);

            return Intersection::SUCCESS;
        }
        else {
            return Intersection::OUT_OF_BOUNDS;
        }

    }
    else if (rayProj > 1e-6) {
        return Intersection::BACKFACE; // reported instead of BEHIND_RAY_ORIGIN
    }
    else {
        return Intersection::PARALLEL;
    }
}

bool Triangle::intersect_plane(const std::vector<Vec3>& vertices, const Ray& ray, float& t, Vec3& p) const {
    const Vec3& v0 = vertices[v[0]];
    const Vec3& v1 = vertices[v[1]];
    const Vec3& v2 = vertices[v[2]];

    // Here assuming counter-clockwise order
    Vec3 e0 = v1 - v0;
    Vec3 e1 = v2 - v0;

    float rayProj = dot(ray.direction, normal);

    // if triangle is facing the ray, compute ray-plane intersection
    if (rayProj < -1e-6) {
        float rpDist = dot(normal, v0 - ray.origin);
        t = rpDist / rayProj;
        p = ray.origin + ray.direction * t;
        return true;
    }
    return false;
}

Vec3 Triangle::intersectionNormal(const Scene& scene, float uCoord, float vCoord) const {
        Vec3 result {};
        const Vec3& v2N = scene.vertexNormals[v[2]];
        const Vec3& v0N = scene.vertexNormals[v[0]];
        const Vec3& v1N = scene.vertexNormals[v[1]];
        if (scene.materials[materialIndex].smoothShading) {
            return v0N * (1 - uCoord - vCoord) + v1N * uCoord + v2N * vCoord;
        }
        else {
            return normal;
        }
    }

