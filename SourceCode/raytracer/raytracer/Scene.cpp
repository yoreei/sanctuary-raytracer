#pragma warning( disable : 4365 )

#include "Scene.h"
#include "TraceHit.h"
#include "Triangle.h"
#include "Material.h"
#include "CRTTypes.h"

/**
* Determine best intersection of ray with scene.
*/
bool Scene::isOccluded(const Vec3& start, const Vec3& end) const {
    TraceHit hit{};
    float t = end.length(); // todo: end - start ???????????????
    Ray ray = { start, end.getUnit() };
    for (const Triangle& tri : triangles) {
        tri.intersect(*this, ray, hit);
        auto& material = materials[tri.materialIndex];
        if (hit.successful() && material.type != Material::Type::REFRACTIVE && flower(hit.t, t)) {
            return true;
        }
    }
    // TODO: return dimming factor based on refractive objects intersected!!!!
    return false;
}
void Scene::intersect(const Ray& ray, TraceHit& out) const {
    out.t = std::numeric_limits<float>::max();
    TraceHit hit {};
    for (const Triangle& tri : triangles) {
        // TODO: Separate plane intersection & triangle uv intersection tests?
        tri.intersect(*this, ray, hit);
        if (hit.successful() && hit.t < out.t) {
            out = hit;
        }

        metrics.record(toString(hit.type));
    }
}

/* Compiles all vertices and triangles into a single mesh object */

[[nodiscard]]
bool Scene::bakeObject() {
    for (const auto& tri : triangles) {
        if (tri.v[0] >= vertices.size() || tri.v[1] >= vertices.size() || tri.v[2] >= vertices.size()) {
            std::cerr << "Error loading triangles: vertex index out of bounds\n";
            return false;
        }
    }

    meshObjects.emplace_back(0, triangles.size() - 1);

    return true;
}

void Scene::addObjects(const std::vector<Scene>& scenes)
{
    for (const auto& scene : scenes) {
        size_t verticesPadding = vertices.size();
        vertices.reserve(vertices.size() + scene.vertices.size());
        vertices.insert(vertices.end(), scene.vertices.begin(), scene.vertices.end());

        size_t trianglesPadding = triangles.size();
        triangles.reserve(triangles.size() + scene.triangles.size());
        triangles.insert(triangles.end(), scene.triangles.begin(), scene.triangles.end());

        for (size_t i = trianglesPadding; i < triangles.size(); i++) {
            triangles[i].v[0] += verticesPadding;
            triangles[i].v[1] += verticesPadding;
            triangles[i].v[2] += verticesPadding;
        }

        size_t meshObjectsPadding = meshObjects.size();
        meshObjects.reserve(meshObjects.size() + scene.meshObjects.size());
        meshObjects.insert(meshObjects.end(), scene.meshObjects.begin(), scene.meshObjects.end());

        for (size_t i = meshObjectsPadding; i < meshObjects.size(); ++i) {
            for (size_t j = 0; j < meshObjects[i].triangleIndexes.size(); ++j) {
                meshObjects[i].triangleIndexes[j] += trianglesPadding;
            }
        }

    }
}
