#pragma once

#include <fstream>

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
    Scene (const std::string& name) : fileName(name), metrics(name), animator(*this) {}

    Scene(Scene&&) noexcept = default;
    Scene& operator=(Scene&&) noexcept = default;

    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;

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
    std::vector<Texture> textures {};
    std::vector<Image> bitmaps {}; // TODO: move to texture
    std::vector<Vec3> uvs {};
    
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

    size_t addBitmap(Image&& bitmap);
private:

};
