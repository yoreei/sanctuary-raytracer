#pragma once

#include <fstream>

#include "CRTTypes.h"
#include "Camera.h"
#include "Settings.h"
#include "Triangle.h"
#include "Animation.h"
#include "MeshObject.h"
#include "Metrics.h"
#include "Image.h"
#include "Light.h"

#include "json.hpp"

using json = nlohmann::json;

class Scene
{
public:
    Camera camera {};
    Settings settings {};
    mutable Metrics metrics {};

    /* meshObjects reference trinagles. Triangles reference vertices */
    std::vector<Vec3> vertices {};
    std::vector<Triangle> triangles {};
    std::vector<MeshObject> meshObjects {};
    std::vector<Light> lights {};
    
    Color bgColor = Color{0, 0, 0};

    bool isOccluded(const Vec3& start, const Vec3& end) const {
        Triangle::IntersectionData xData {};
        float t = end.length();
        Ray ray = { start, end.normalize() };
        for (const Triangle& tri : triangles) {
            Intersection x = tri.intersect(vertices, ray, xData);
            if (x == Intersection::SUCCESS && flower(xData.t, t)) {
                return true;
            }
        }
        return false;
    }

    /* Compiles all vertices and triangles into a single mesh object */
    [[nodiscard]] bool bakeObject() {
        for (const auto& tri : triangles) {
            if (tri.v[0] >= vertices.size() || tri.v[1] >= vertices.size() || tri.v[2] >= vertices.size()) {
                std::cerr << "Error loading triangles: vertex index out of bounds\n";
                return false;
            }
        }

        meshObjects.emplace_back(0, triangles.size() - 1);

        return true;
    }

    void addObjects(const std::vector<Scene>& scenes)
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
                for (size_t j = 0; j < meshObjects[i].triangles.size(); ++j) {
                    meshObjects[i].triangles[j] += trianglesPadding;
                }
            }

        }
    }

private:

};
