#pragma once
#include "include/CRTTypes.h"
#include "include/TraceHit.h"

class Scene;
class Triangle {
public:
    size_t v[3]; // indices into the vertex array
    size_t materialIndex = 0;

    Triangle(const std::vector<Vec3>& vertices, size_t v0, size_t v1, size_t v2, size_t _materialIndex);

    void intersect(const Scene& scene, const Ray& ray, size_t triRef, TraceHit& hit) const;

    /* Quick line-triangle intersect that returns only a bool. Used for occlusion testing.
    *  @return: true also for backside */
    bool fastIntersect(const Scene& scene, const Vec3& start, const Vec3& end) const;

    bool intersect_plane(const std::vector<Vec3>& vertices, const Ray& ray, float& t, Vec3& p) const;

    float area(const std::vector<Vec3>& vertices) const;

    bool hasVertex(size_t vertexIndex) const;

    void translate(const Vec3& translation, std::vector<Vec3>& vertices) const;

    Vec3 calculateNormal(const std::vector<Vec3>& vertices) const;

    void buildAABB(const std::vector<Vec3>& vertices, Vec3* bounds) const;

    std::string toString(const std::vector<Vec3>& vertices) const;

    [[nodiscard]] Vec3 getNormal() const;
private:
    Vec3 normal{};

    Vec3 hitNormal(const Scene& scene, const TraceHit& hit) const;

    void computeHit(const Scene& scene, const Ray& ray, float rProj, TraceHit& hit) const;

    void swapFace();

    /* Used for quick line-triangle intersection testing
    * @return: true if sign is positive. False if sign is negative or zero */
    bool signOfVolume(const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& d) const;

    static TraceHitType getTraceHitType(const Vec3& n, const Vec3& rayDir);

    Triangle swappedTriangle() const;

    /* for DEBUG builds */
    static void assertHit(const Scene& scene, const TraceHit& hit);
};

