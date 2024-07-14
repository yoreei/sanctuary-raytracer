#pragma once

#include <fstream>

#include "Animator.h"
#include "CRTTypes.h"
#include "Camera.h"
#include "Settings.h"
#include "Triangle.h"
#include "Animation.h"
#include "MeshObject.h"
#include "Metrics.h"
#include "Light.h"
#include "Material.h"

#include "json.hpp"

using json = nlohmann::json;

class TraceHit;
class Scene
{
public:
    Scene (const std::string& name) : fileName(name), metrics(name), animator(*this) {}
    std::string fileName = "";
    Camera camera {};
    Settings settings {};
    mutable Metrics metrics;
    Animator animator;

    /* meshObjects reference trinagles. Triangles reference vertices */
    std::vector<Vec3> vertices {};
    std::vector<Triangle> triangles {};
    std::vector<Vec3> vertexNormals {};
    std::vector<MeshObject> meshObjects {};
    std::vector<Material> materials {};
    std::vector<Light> lights {};
    
    Vec3 bgColor = {0.f, 0.f, 0.f};

    bool isOccluded(const Vec3& start, const Vec3& end) const;

    void intersect(const Ray& ray, TraceHit& out) const;

    /* Compiles all vertices and triangles into a single mesh object */
    [[nodiscard]] bool bakeObject();

    void addObjects(const std::vector<Scene>& scenes);

    /*
    * Show debug visualizations for lights in the scene
    */
    void showLightDebug();

    /*
    * populates `vertexNormals` with the average normal of all attached triangles. Important for smooth shading
    */
    void generateVertexNormals();

    void genAttachedTriangles(size_t vertexIndex, std::vector<size_t>& attachedTriangles);
private:

};
