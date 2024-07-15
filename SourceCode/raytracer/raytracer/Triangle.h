#pragma once
#include "CRTTypes.h"
#include "TraceHit.h"

class Scene;
class Triangle {
public:
    size_t v[3]; // indices into the vertex array
    size_t materialIndex = 0;

    Triangle(const std::vector<Vec3>& vertices, size_t v0, size_t v1, size_t v2, size_t _materialIndex);

    Vec3 calculateNormal(const std::vector<Vec3>& vertices) const;

    [[nodiscard]] Vec3 getNormal() const;

    float area(const std::vector<Vec3>& vertices) const;

    std::string toString(const std::vector<Vec3>& vertices) const;

    bool hasVertex(size_t vertexIndex) const;

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

    void intersect(const Scene& scene, const Ray& ray, TraceHit& hit) const;

    bool boolIntersect(const Scene& scene, const Vec3& start, const Vec3& end) const;

    void computeHit(const Scene& scene, const Ray& ray, float rProj, TraceHit& hit) const;

    static TraceHitType getTraceHitType(const Vec3& n, const Vec3& rayDir);

    static void assertTraceHitType(TraceHitType type, const Scene& scene, size_t materialIndex);

    Triangle swappedTriangle() const;

    void swapFace();

    bool intersect_plane(const std::vector<Vec3>& vertices, const Ray& ray, float& t, Vec3& p) const;

    Vec3 hitNormal(const Scene& scene, float uCoord, float vCoord) const;

    void translate(const Vec3& translation, std::vector<Vec3>& vertices) const;

    bool signOfVolume(const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& d) const;

private:
    Vec3 normal{};
};

