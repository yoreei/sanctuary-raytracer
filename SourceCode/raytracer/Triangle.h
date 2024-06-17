#pragma once
#include "CRTTypes.h"

// hw4 Task 1
class Triangle {
public:
    Vec3 v0, v1, v2;

    Triangle() : v0(Vec3()), v1(Vec3()), v2(Vec3()) {}

    Triangle(const Vec3& v0, const Vec3& v1, const Vec3& v2) : v0(v0), v1(v1), v2(v2) {}

    Vec3 normal() const {
        Vec3 e1 = v1 - v0;
        Vec3 e2 = v2 - v0;
        return e1.cross(e2).normalize();
    }

    float area() const {
        Vec3 e1 = v1 - v0;
        Vec3 e2 = v2 - v0;
        return e1.cross(e2).length() * 0.5f;
    }

    Triangle transform(const Matrix3x3& mat) const {
        Vec3 newV0 = mat * v0;
        Vec3 newV1 = mat * v1;
        Vec3 newV2 = mat * v2;
        return Triangle(newV0, newV1, newV2);
    }

    std::string toString() const {
        return "Triangle: {" + v0.toString() + ", " + v1.toString() + ", " + v2.toString() + "}";
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
        Vec3 e0 = v1 - v0;
        Vec3 e1 = v2 - v0;
        Vec3 tri_normal = cross(e0, e1).normalize();

        float rayProj = dot(ray.direction, tri_normal);

        // if triangle is facing the ray, compute ray-plane intersection
        if (rayProj < -1e-6) {
            float rpDist = dot(tri_normal, v0 - ray.origin); // was  ray.origin - v0
            t = rpDist / rayProj;
            p = ray.origin + ray.direction * t;

            // check if `p` is inside triangle
            Vec3 c0 = cross(e0, p - v0);
            Vec3 c1 = cross(v2 - v1, p - v1);
            Vec3 c2 = cross(v0 - v2, p - v2);

            return dot(tri_normal, c0) > 1e-6 &&
                   dot(tri_normal, c1) > 1e-6 &&
                   dot(tri_normal, c2) > 1e-6;
        }
        else {
            std::cout << "Back-culling triggered for triangle: " << toString() << std::endl;
        }
        return false;
    }

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
    bool intersect(const Ray& ray, float& t, Vec3& p, Vec3& n, float& u, float& v) const {
        // Here assuming counter-clockwise order
        Vec3 e0 = v1 - v0;
        Vec3 e1 = v2 - v0;
        Vec3 plane_ortho = cross(e0, e1);
        n = plane_ortho.normalize();

        float rayProj = dot(ray.direction, n);

        // if triangle is facing the ray, compute ray-plane intersection
        if (rayProj < -1e-6) {
            float rpDist = dot(n, v0 - ray.origin);
            t = rpDist / rayProj;
            p = ray.origin + ray.direction * t;

            // check if `p` is inside triangle
            Vec3 v0p = p - v0;
            Vec3 e2 = v2 - v1;
            Vec3 c0 = cross(e0, v0p);
            Vec3 c1 = cross(v0p, e1);
            Vec3 c2 = cross(e2, p - v1);

            bool inside = dot(n, c0) > 1e-6 &&
                   dot(n, c1) > 1e-6 &&
                   dot(n, c2) > 1e-6;

            if (inside) {
                float area_inv = 1.f / plane_ortho.length();
                u = c0.length() * area_inv;
                v = c1.length() * area_inv;
                return true;
            }
        }
        else {
            std::cout << "Back-culling triggered for triangle: " << toString() << std::endl;
        }
        return false;
    }

    bool intersect_plane(const Ray& ray, float& t, Vec3& p) const {
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

    /* From scratchapixel */
    bool intersect_plane2(const Ray& ray, float& t, Vec3& p) const
    {
        Vec3 e0 = v1 - v0;
        Vec3 e1 = v2 - v0;
        Vec3 n = cross(e0, e1).normalize();
        Vec3 p0 = v0;
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