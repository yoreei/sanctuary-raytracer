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

void Triangle::intersect(const Scene& scene, const Ray& ray, Intersection& xData) const {

    float rayProj = ray.direction.dot(normal);
    bool planeFacingRay = rayProj < -1e-6;

    if (planeFacingRay) {
        computeXData(scene, ray, rayProj, xData);
    }
    else {
        auto& material = scene.materials[materialIndex];
        if (material.type == Material::Type::REFRACTIVE) {
            Triangle swapT = swappedTriangle();
            swapT.intersect(scene, ray, xData);
            if (xData.type == IntersectionType::SUCCESS) {
                xData.type = IntersectionType::INSIDE_REFRACTIVE;
                xData.n = -xData.n;
                assert(dot(xData.n, ray.direction) > 1e-6); // ray is exiting refractive material
            }
        }
        else {
            xData.type = IntersectionType::PLANE_BACKFACE;
        }
    }
}

/**
* @param n: normal of the triangle to use instead of this->normal (for refraction)
*/
void Triangle::computeXData(const Scene& scene, const Ray& ray, float rProj, Intersection& xData) const {
    const Vec3& n = normal; // refactoring helper. TODO: remove
    const Vec3& v0 = scene.vertices[v[0]];
    const Vec3& v1 = scene.vertices[v[1]];
    const Vec3& v2 = scene.vertices[v[2]];

    // Here assuming counter-clockwise order
    Vec3 e0 = v1 - v0;
    Vec3 e1 = v2 - v0;
    float rpDist = dot(n, v0 - ray.origin); // Ray-Plane distance
    xData.t = rpDist / rProj;
    if (xData.t < -1e-6) {
        xData.type = IntersectionType::PLANE_BEHIND_RAY_ORIGIN;
        return;
    }
    xData.p = ray.origin + ray.direction * xData.t;

    // check if `p` is inside triangle
    Vec3 v0p = xData.p - v0;
    Vec3 e2 = v2 - v1;
    Vec3 c0 = cross(e0, v0p);
    Vec3 c1 = cross(v0p, e1);
    Vec3 c2 = cross(e2, xData.p - v1);

    bool inside = dot(n, c0) > 1e-6 &&
        dot(n, c1) > 1e-6 &&
        dot(n, c2) > 1e-6;

    if (inside) {
        float area_inv = 1.f / e0.crossLength(e1);
        xData.u = c0.length() * area_inv;
        xData.v = c1.length() * area_inv;
        xData.materialIndex = this->materialIndex;
        xData.n = intersectionNormal(scene, xData.u, xData.v);
        xData.type = IntersectionType::SUCCESS;
        return;
    }
    else {
        xData.type = IntersectionType::OUT_OF_BOUNDS;
        return;
    }
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

Vec3 Triangle::intersectionNormal(const Scene& scene, float uCoord, float vCoord) const {
    Vec3 result{};
    if (scene.materials[materialIndex].smoothShading) {
        const Vec3& v2N = scene.vertexNormals[v[2]];
        const Vec3& v0N = scene.vertexNormals[v[0]];
        const Vec3& v1N = scene.vertexNormals[v[1]];
        return v0N * (1 - uCoord - vCoord) + v1N * uCoord + v2N * vCoord;
    }
    else {
        return normal;
    }
}

