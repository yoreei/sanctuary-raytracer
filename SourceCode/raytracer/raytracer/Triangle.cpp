#pragma warning( disable : 4365 )

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

Triangle::Triangle(const std::vector<Vec3>& vertices, size_t v0, size_t v1, size_t v2, size_t _materialIndex)
{
    v[0] = v0;
    v[1] = v1;
    v[2] = v2;
    materialIndex = _materialIndex;
    normal = calculateNormal(vertices);
}

Vec3 Triangle::calculateNormal(const std::vector<Vec3>& vertices) const {
    Vec3 out{};
    const Vec3& v0 = vertices[v[0]];
    const Vec3& v1 = vertices[v[1]];
    const Vec3& v2 = vertices[v[2]];

    Vec3 e1 = v1 - v0;
    Vec3 e2 = v2 - v0;

    e1.cross(e2, out);
    out.normalize();
    return out;
}

[[nodiscard]]
Vec3 Triangle::getNormal() const {
    return normal;
}

float Triangle::area(const std::vector<Vec3>& vertices) const {
    const Vec3& v0 = vertices[v[0]];
    const Vec3& v1 = vertices[v[1]];
    const Vec3& v2 = vertices[v[2]];

    Vec3 e1 = v1 - v0;
    Vec3 e2 = v2 - v0;
    return e1.crossLength(e2) * 0.5f;
}

std::string Triangle::toString(const std::vector<Vec3>& vertices) const {
    const Vec3& v0 = vertices[v[0]];
    const Vec3& v1 = vertices[v[1]];
    const Vec3& v2 = vertices[v[2]];

    return "Triangle: {" + v0.toString() + ", " + v1.toString() + ", " + v2.toString() + "}";
}

bool Triangle::hasVertex(size_t vertexIndex) const {
    return v[0] == vertexIndex || v[1] == vertexIndex || v[2] == vertexIndex;
}

void Triangle::intersect(const Scene& scene, const Ray& ray, TraceHit& hit) const {
    assert(fEqual(ray.direction.lengthSquared(), 1.f));

    float rayProj = ray.direction.dot(normal);

    if (rayProj < -1e-6) { // normal is facing ray
        computeHit(scene, ray, rayProj, hit);
    }
    else if (rayProj > 1e-6) { // normal is facing away from ray
        auto& material = scene.materials[materialIndex];
        if (material.type == Material::Type::REFRACTIVE) {
            Triangle swapT = swappedTriangle();
            swapT.intersect(scene, ray, hit);
            if (hit.type == TraceHitType::SUCCESS) {
                hit.type = TraceHitType::INSIDE_REFRACTIVE;
                hit.n = -hit.n;
                assert(dot(hit.n, ray.direction) > 1e-6); // ray is exiting refractive material
            }
        }
        else {
            hit.type = TraceHitType::PLANE_BACKFACE;
        }
    }
    else {
        hit.type = TraceHitType::PARALLEL;
    }
}

/*
* Quick line-triangle intersect that returns only a bool. Used for occlusion testing.
* @return: true also for backside
*/
bool Triangle::boolIntersect(const Scene& scene, const Vec3& start, const Vec3& end) const
{
    const Vec3& v0 = scene.vertices[v[0]];
    const Vec3& v1 = scene.vertices[v[1]];
    const Vec3& v2 = scene.vertices[v[2]];

    if (signOfVolume(start, v0, v1, v2) != signOfVolume(end, v0, v1, v2)) {
        bool pyr3 = signOfVolume(start, end, v0, v1);
        if (pyr3 == signOfVolume(start, end, v1, v2) && pyr3 == signOfVolume(start, end, v2, v0)) {
            return true;
        }
    }
    return false;
}

/**
* @param n: normal of the triangle to use instead of this->normal (for refraction)
*/
void Triangle::computeHit(const Scene& scene, const Ray& ray, float rProj, TraceHit& hit) const {
    const Vec3& n = normal; // refactoring helper. TODO: remove
    const Vec3& v0 = scene.vertices[v[0]];
    const Vec3& v1 = scene.vertices[v[1]];
    const Vec3& v2 = scene.vertices[v[2]];

    // Here assuming counter-clockwise order
    Vec3 e0 = v1 - v0;
    Vec3 e1 = v2 - v0;
    float rpDist = dot(n, v0 - ray.origin); // Ray-Plane distance
    hit.t = rpDist / rProj;
    if (hit.t < -1e-6) {
        hit.type = TraceHitType::PLANE_BEHIND_RAY_ORIGIN;
        return;
    }
    hit.p = ray.origin + ray.direction * hit.t;

    // check if `p` is inside triangle
    Vec3 v0p = hit.p - v0;
    Vec3 e2 = v2 - v1;
    Vec3 c0 = cross(e0, v0p);
    Vec3 c1 = cross(v0p, e1);
    Vec3 c2 = cross(e2, hit.p - v1);

    bool inside = dot(n, c0) > -1e-6 && // TODO: optimize dot away
        dot(n, c1) > -1e-6 &&
        dot(n, c2) > -1e-6;

    if (inside) {
        float area_inv = 1.f / e0.crossLength(e1);
        hit.baryU = c1.length() * area_inv;
        hit.baryV = c0.length() * area_inv;
        hit.u;
        hit.v;
        zzz
        hit.materialIndex = this->materialIndex;
        hit.n = hitNormal(scene, hit.u, hit.v);
        hit.type = getTraceHitType(hit.n, ray.direction);
        assertTraceHitType(hit.type, scene, materialIndex);
        return;
    }
    else {
        hit.type = TraceHitType::OUT_OF_BOUNDS;
        return;
    }
}

TraceHitType Triangle::getTraceHitType(const Vec3& n, const Vec3& rayDir) {
    float rProj = dot(rayDir, n);
    if (rProj < -epsilon) {
        return TraceHitType::SUCCESS;
    }
    else if (rProj > epsilon) {
        return TraceHitType::SMOOTH_SHADING_BACKFACE;
    }
    else {
        return TraceHitType::SMOOTH_SHADING_PARALLEL;
    }
}

void Triangle::assertTraceHitType(TraceHitType type, const Scene& scene, size_t materialIndex) {
    // This function should be optimized away by the compiler in release builds
    // Suppress unused parameter warnings:
    type;
    scene;
    materialIndex;
#ifndef NDEBUG
    if (type == TraceHitType::SMOOTH_SHADING_BACKFACE || type == TraceHitType::SMOOTH_SHADING_PARALLEL) {
        assert(scene.materials[materialIndex].smoothShading);
    }
#endif
}

Triangle Triangle::swappedTriangle() const
{
    Triangle t = *this;
    t.swapFace();
    return t;
}

void Triangle::swapFace() {
    auto v2buff = v[2];
    v[2] = v[1];
    v[1] = v2buff;
    normal = -normal;
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

Vec3 Triangle::hitNormal(const Scene& scene, float uCoord, float vCoord) const {
    Vec3 result{};
    if (scene.materials[materialIndex].smoothShading) {
        const Vec3& v0N = scene.vertexNormals[v[0]];
        const Vec3& v1N = scene.vertexNormals[v[1]];
        const Vec3& v2N = scene.vertexNormals[v[2]];
        return v0N * (1 - uCoord - vCoord) + v1N * uCoord + v2N * vCoord;
    }
    else {
        return normal;
    }
}

void Triangle::translate(const Vec3& translation, std::vector<Vec3>& vertices) const
{
   for (size_t i = 0; i < 3; ++i) {
        vertices[v[i]] += translation;
    }
}


/*
* Used for quick line-triangle intersection testing
* @return: true if sign is positive. False if sign is negative or zero
*/
bool Triangle::signOfVolume(const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& d) const {
    return dot(cross(b-a,c-a),d-a) > 0.f;
}


