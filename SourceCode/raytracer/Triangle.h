#pragma once
#include "CRTTypes.h"
#include "Intersection.h"

// hw4 Task 1
class Triangle {
public:
    size_t v[3]; // indices into the vertex array

    Triangle(size_t v0, size_t v1, size_t v2)
    {
        v[0] = v0;
        v[1] = v1;
        v[2] = v2;
    }

    static Triangle fromVertices(const Vec3& v0, const Vec3& v1, const Vec3& v2, std::vector<Vec3>& vertices) {
        size_t i0 = vertices.size();
        size_t i1 = i0 + 1;
        size_t i2 = i1 + 1;

        vertices.push_back(v0);
        vertices.push_back(v1);
        vertices.push_back(v2);

        return Triangle(i0, i1, i2);
    }

    void normal(const std::vector<Vec3>& vertices, Vec3& out) const {
        const Vec3& v0 = vertices[v[0]];
        const Vec3& v1 = vertices[v[1]];
        const Vec3& v2 = vertices[v[2]];

        Vec3 e1 = v1 - v0;
        Vec3 e2 = v2 - v0;

        e1.cross(e2, out);
        out = out.normalize();
    }

    float area(const std::vector<Vec3>& vertices) const {
        const Vec3& v0 = vertices[v[0]];
        const Vec3& v1 = vertices[v[1]];
        const Vec3& v2 = vertices[v[2]];

        Vec3 e1 = v1 - v0;
        Vec3 e2 = v2 - v0;
        return e1.crossLength(e2) * 0.5f;
    }

    std::string toString(const std::vector<Vec3>& vertices) const {
        const Vec3& v0 = vertices[v[0]];
        const Vec3& v1 = vertices[v[1]];
        const Vec3& v2 = vertices[v[2]];

        return "Triangle: {" + v0.toString() + ", " + v1.toString() + ", " + v2.toString() + "}";
    }

    struct IntersectionData {
        float t = FLT_MAX;    // Distance
        Vec3 p = {0, 0, 0};     // Intersection Point
        Vec3 n = {0, 0, 0};     // Normal at Point
        float u = 0.f;    // First Barycentric Base
        float v = 0.f;    // Second Barycentric Base

        bool intersectionSuccessful() const {
            return t < FLT_MAX;
        }
    };

    /*
    * @Danny search OneNote 'Triangle Intersect'
    * Input: ray, rayProj
    * Input: rayProj: projection of ray.direction onto camera direction
    * Output: t: distance from ray.origin to the intersection point
    * Output: p: intersection point
    * Output: n: triangle normal (unit)
    * OUtput: u, v: triangle-space coordinates of intersection for UV mapping
    * return: true if the ray intersects the triangle
    */

    Intersection intersect(const std::vector<Vec3>& vertices, const Ray& ray, IntersectionData& out) const {
        const Vec3& v0 = vertices[v[0]];
        const Vec3& v1 = vertices[v[1]];
        const Vec3& v2 = vertices[v[2]];

        // Here assuming counter-clockwise order
        Vec3 e0 = v1 - v0;
        Vec3 e1 = v2 - v0;
        Vec3 plane_ortho = cross(e0, e1);
        out.n = plane_ortho.normalize();

        float rayProj = ray.direction.dot(out.n);

        // if triangle facing ray
        if (rayProj < -1e-6) {
            float rpDist = dot(out.n, v0 - ray.origin);
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

            bool inside = dot(out.n, c0) > 1e-6 &&
                   dot(out.n, c1) > 1e-6 &&
                   dot(out.n, c2) > 1e-6;

            if (inside) {
                float area_inv = 1.f / plane_ortho.length();
                out.u = c0.length() * area_inv;
                out.v = c1.length() * area_inv;
                return Intersection::SUCCESS;
            }
            else {
                return Intersection::OUT_OF_BOUNDS;
            }

        }
        else if (rayProj > 1e-6){
            return Intersection::BACKFACE; // reported instead of BEHIND_RAY_ORIGIN
        }
        else {
            return Intersection::PARALLEL;
        }
    }

    bool intersect_plane(const std::vector<Vec3>& vertices, const Ray& ray, float& t, Vec3& p) const {
        const Vec3& v0 = vertices[v[0]];
        const Vec3& v1 = vertices[v[1]];
        const Vec3& v2 = vertices[v[2]];

        // Here assuming counter-clockwise order
        Vec3 e0 = v1 - v0;
        Vec3 e1 = v2 - v0;
        Vec3 tri_normal = cross(e0, e1).normalize();

        float rayProj = dot(ray.direction, tri_normal);

        // if triangle is facing the ray, compute ray-plane intersection
        if (rayProj < -1e-6) {
            float rpDist = dot(tri_normal, v0 - ray.origin);
            t = rpDist / rayProj;
            p = ray.origin + ray.direction * t;
            return true;
        }
        return false;
    }
};