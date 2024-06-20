#pragma once
#include "CRTTypes.h"
#include "Intersection.h"

// hw4 Task 1
class Triangle {
public:
    Vec3 v[3] = {Vec3(), Vec3(), Vec3()};

    Triangle() = default;

    Triangle(const Vec3& _v0, const Vec3& _v1, const Vec3& _v2) : v{_v0, _v1, _v2} {}

    Vec3 normal() const {
        Vec3 e1 = v[1] - v[0];
        Vec3 e2 = v[2] - v[0];
        return e1.cross(e2).normalize();
    }

    float area() const {
        Vec3 e1 = v[1] - v[0];
        Vec3 e2 = v[2] - v[0];
        return e1.cross(e2).length() * 0.5f;
    }

    Triangle transform(const Matrix3x3& mat) const {
        Vec3 newV0 = mat * v[0];
        Vec3 newV1 = mat * v[1];
        Vec3 newV2 = mat * v[2];
        return Triangle(newV0, newV1, newV2);
    }

    std::string toString() const {
        return "Triangle: {" + v[0].toString() + ", " + v[1].toString() + ", " + v[2].toString() + "}";
    }

    /*
    * Input: ray, rayProj
    * Input: rayProj: projection of ray.direction onto camera direction
    * Output: t: distance from ray.origin to the intersection point
    * Output: p: intersection point
    * return: true if the ray intersects the triangle
    */
    bool intersect_simple(const Ray& ray, float& t, Vec3& p) const {
        // Here assuming counter-clockwise order
        Vec3 e0 = v[1] - v[0];
        Vec3 e1 = v[2] - v[0];
        Vec3 tri_normal = cross(e0, e1).normalize();

        float rayProj = dot(ray.direction, tri_normal);

        // if triangle is facing the ray, compute ray-plane intersection
        if (rayProj < -1e-6) {
            float rpDist = dot(tri_normal, v[0] - ray.origin); // was  ray.origin - v[0]
            t = rpDist / rayProj;
            p = ray.origin + ray.direction * t;

            // check if `p` is inside triangle
            Vec3 c0 = cross(e0, p - v[0]);
            Vec3 c1 = cross(v[2] - v[1], p - v[1]);
            Vec3 c2 = cross(v[0] - v[2], p - v[2]);

            return dot(tri_normal, c0) > 1e-6 &&
                   dot(tri_normal, c1) > 1e-6 &&
                   dot(tri_normal, c2) > 1e-6;
        }
        else {
            std::cout << "Back-culling triggered for triangle: " << toString() << std::endl;
        }
        return false;
    }

    struct IntersectionData {
        float t = FLT_MAX;    // Distance
        Vec3 p = {0, 0, 0};     // Intersection Point
        Vec3 n = {0, 0, 0};     // Normal at Point
        float u = 0.f;    // First Barycentric Base
        float v = 0.f;    // Second Barycentric Base
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

    Intersection intersect(const Ray& ray, IntersectionData& out) const {
        // Here assuming counter-clockwise order
        Vec3 e0 = v[1] - v[0];
        Vec3 e1 = v[2] - v[0];
        Vec3 plane_ortho = cross(e0, e1);
        out.n = plane_ortho.normalize();

        float rayProj = dot(ray.direction, out.n);

        // if triangle facing ray
        if (rayProj < -1e-6) {
            float rpDist = dot(out.n, v[0] - ray.origin);
            out.t = rpDist / rayProj;
            if (out.t < -1e-6) {
                return Intersection::BEHIND_RAY_ORIGIN; // Not reachable?
            }
            out.p = ray.origin + ray.direction * out.t;

            // check if `p` is inside triangle
            Vec3 v0p = out.p - v[0];
            Vec3 e2 = v[2] - v[1];
            Vec3 c0 = cross(e0, v0p);
            Vec3 c1 = cross(v0p, e1);
            Vec3 c2 = cross(e2, out.p - v[1]);

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

    bool intersect_plane(const Ray& ray, float& t, Vec3& p) const {
        // Here assuming counter-clockwise order
        Vec3 e0 = v[1] - v[0];
        Vec3 e1 = v[2] - v[0];
        Vec3 tri_normal = cross(e0, e1).normalize();

        float rayProj = dot(ray.direction, tri_normal);

        // if triangle is facing the ray, compute ray-plane intersection
        if (rayProj < -1e-6) {
            float rpDist = dot(tri_normal, v[0] - ray.origin);
            t = rpDist / rayProj;
            p = ray.origin + ray.direction * t;
            return true;
        }
        return false;
    }

    /* From scratchapixel */
    bool intersect_plane2(const Ray& ray, float& t, Vec3& p) const
    {
        Vec3 e0 = v[1] - v[0];
        Vec3 e1 = v[2] - v[0];
        Vec3 n = cross(e0, e1).normalize();
        Vec3 p0 = v[0];
        Vec3 l0 = ray.origin;
        Vec3 l = ray.direction;

        // Assuming vectors are all normalized
        float denom = dot(n, l);
        if (denom < -1e-6) {
            Vec3 p0l0 = p0 - l0;
            t = dot(p0l0, n) / denom; 
            p = l0 + l * t;
            return (t >= 0);
        }

        return false;
    }
};