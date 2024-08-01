#pragma once

#include <vector>
#include "include/SceneObject.h"

class Triangle;
class Material;
class Scene;

class MeshObject : public SceneObject
{
public:
    std::vector<size_t> triangleIndexes; // indices into the triangle array

    /* Construct a mesh object from a range of triangles. `l` and `u` inclusive. */
    MeshObject(size_t l, size_t u);

    static MeshObject fromTriangles(const std::vector<size_t>& triangleIndexes);

    /* Move vertices by the given translation vector */
    void translate(const std::vector<Triangle>& triangles, const Vec3& translation, std::vector<Vec3>& vertices);

    Material& getMaterial(Scene& scene) const;

    void setMaterialIdx(Scene& scene, size_t materialIdx);

};