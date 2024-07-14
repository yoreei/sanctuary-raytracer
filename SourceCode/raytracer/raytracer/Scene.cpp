#pragma warning( disable : 4365 )

#include "Scene.h"
#include "TraceHit.h"
#include "Triangle.h"
#include "Material.h"
#include "CRTTypes.h"
#include "CRTSceneLoader.h"
#include "Light.h"
#include "MeshObject.h"

/**
* @param start: location vector
* @param end: location vector
* Determine best intersection of ray with scene.
*/
bool Scene::isOccluded(const Vec3& start, const Vec3& end) const {
    // todo remove Vec3 occlusionLine = end - start;
    // todo remove float maxDistanceSq = occlusionLine.lengthSquared();
    // todo remove Ray ray = { start, occlusionLine.getUnit() };
    for (const Triangle& tri : triangles) {
        TraceHit hit{}; // todo move up
        auto& material = materials[tri.materialIndex];
        if (!material.occludes) {
            continue;
        }

        if (tri.boolIntersect(*this, start, end)) {
           return true;
        }
        else {
            continue;
        }
    }
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

void Scene::showLightDebug() {
    std::vector<Scene> lightObjects;
    Image _fakeImage {}; // throwaway image
    for (const auto& light : lights) {
        Scene lightBallScene {"LightBall"};
        CRTSceneLoader::loadCrtscene("scenes/props/lightBall.crtscene", lightBallScene, _fakeImage);
        assert(lightBallScene.meshObjects.size() == 1);
        const Vec3& translation = light.pos;
        MeshObject& meshObject = lightBallScene.meshObjects[0];
        meshObject.translate(lightBallScene.triangles, translation, lightBallScene.vertices);
        lightObjects.push_back(lightBallScene);
    }
    addObjects(lightObjects);
}

void Scene::generateVertexNormals() {
    // Vec3{0.f, 0.f, 0.f} is important for summation
    vertexNormals.resize(vertices.size(), Vec3{0.f, 0.f, 0.f});
    for(size_t i = 0; i < vertices.size(); ++i) {
        std::vector<size_t> attachedTriangles {};
        genAttachedTriangles(i, attachedTriangles);
        for(size_t triIndex : attachedTriangles) {
            vertexNormals[i] += triangles[triIndex].getNormal();
        }
    }

    for(size_t i = 0; i < vertexNormals.size(); ++i) {
        auto& normal = vertexNormals[i];
        normal.normalize();
    }
}

/**
 * @brief Generate a list of triangle indexes that are attached to a vertex
 * 
 * @param scene Scene object
 * @param vertexIndex Index of the vertex
 * @param attachedTriangles Output list of triangle indexes
 */
void Scene::genAttachedTriangles(size_t vertexIndex, std::vector<size_t>& attachedTriangles) {
    for(size_t i = 0; i < triangles.size(); ++i) {
        const Triangle& tri = triangles[i];
        if(tri.hasVertex(vertexIndex)) {
            attachedTriangles.push_back(i);
        }
    }
}
