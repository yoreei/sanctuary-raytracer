#pragma once

#include <fstream>

#include "KDTreeNode.h"
#include "Animator.h"
#include "Animation.h"
#include "CRTTypes.h"
#include "Camera.h"
#include "Image.h"
#include "Light.h"
#include "MeshObject.h"
#include "Material.h"
#include "Metrics.h"
#include "Settings.h"
#include "Triangle.h"
#include "Texture.h"

#include "json.hpp"

using json = nlohmann::json;

class TraceHit;
class Scene
{
public:
    Scene(const std::string& name, const Settings& settings) : fileName(name), metrics(name), animator(*this), settings(settings) {}

    Scene(Scene&&) noexcept = default;
    Scene& operator=(Scene&&) noexcept = default;

    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;

    std::string fileName = "";
    Camera camera{};
    const Settings settings;
    mutable Metrics metrics{};
    Animator animator;

    /* meshObjects reference trinagles. Triangles reference vertices */
    std::vector<Light> lights {};
    std::vector<MeshObject> meshObjects {};
    std::vector<Material> materials {};
    std::vector<Texture> textures {};
    std::vector<Triangle> triangles {};
    std::vector<AABB> triangleAABBs {};
    std::vector<Vec3> uvs {};
    std::vector<Vec3> vertexNormals {};
    std::vector<Vec3> vertices {};
    // IMPORTANT: update `Scene::addObjects()` if adding new members
    // IMPORTANT: keep alphabetical order

    Vec3 bgColor = { 0.f, 0.f, 0.f };

    bool isOccluded(const Vec3& start, const Vec3& end) const;

    void intersect(const Ray& ray, TraceHit& out) const;

    /* @brief: Marks scene dirty. Do not forget to `build` the scene after addObject! */
    MeshObject& addObject(
        std::vector<Vec3>& objVertices,
        std::vector<Triangle>& objTriangles,
        std::vector<Vec3>& objVertexNormals,
        std::vector<Vec3>& objUvs);

    void merge(const Scene& other);

    /*
    * Show debug visualizations for lights in the scene
    */
    void showLightDebug();

    /* @brief: build acceleration structures. Marks scene clean. */
    void build();

    struct Timers {
        static constexpr const char* buildScene = "buildScene";
    };

    bool getIsDirty() const { return isDirty; }

private:
    KDTreeNode accelStruct {};
    bool isDirty = true; /* Scene is dirty if objects are added or removed */

    bool triangleAABBsDirty = true;
};
