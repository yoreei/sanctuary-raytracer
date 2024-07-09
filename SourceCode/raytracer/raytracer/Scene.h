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
#include "Image.h"
#include "Light.h"
#include "Material.h"
#include "Intersection.h"

#include "json.hpp"

using json = nlohmann::json;

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
    
    //Material bgMaterial = {};
    Vec3 bgColor = {0.f, 0.f, 0.f};

    bool isOccluded(const Vec3& start, const Vec3& end) const;

    void intersect(const Ray& ray, Intersection& out) const;

    /* Compiles all vertices and triangles into a single mesh object */
    [[nodiscard]] bool bakeObject();

    void addObjects(const std::vector<Scene>& scenes);

private:

};
