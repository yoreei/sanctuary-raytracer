#pragma once
#include "CRTTypes.h"
#include "Intersection.h"

class Scene;
class Triangle {
public:
    size_t v[3]; // indices into the vertex array
    size_t materialIndex = 0;

    Triangle(const std::vector<Vec3>& vertices, size_t v0, size_t v1, size_t v2, size_t _materialIndex)
    {
        v[0] = v0;
        v[1] = v1;
        v[2] = v2;
        materialIndex = _materialIndex;
        normal = calculateNormal(vertices);
    }

    Vec3 calculateNormal(const std::vector<Vec3>& vertices) const {
        Vec3 out {};
        const Vec3& v0 = vertices[v[0]];
        const Vec3& v1 = vertices[v[1]];
        const Vec3& v2 = vertices[v[2]];

        Vec3 e1 = v1 - v0;
        Vec3 e2 = v2 - v0;

        e1.cross(e2, out);
        out.normalize();
        return out;
    }

    [[nodiscard]] Vec3 getNormal() const {
        return normal;
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

    bool hasVertex(size_t vertexIndex) const {
        return v[0] == vertexIndex || v[1] == vertexIndex || v[2] == vertexIndex;
    }

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
    Intersection intersect(const Scene& scene, const Ray& ray, IntersectionData& out) const;

    bool intersect_plane(const std::vector<Vec3>& vertices, const Ray& ray, float& t, Vec3& p) const;

    Vec3 intersectionNormal(const Scene& scene, float uCoord, float vCoord) const;

private:
    Vec3 normal {};
};