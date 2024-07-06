#pragma once

#include <vector>

#include "SceneObject.h"

class MeshObject : public SceneObject
{
public:
    std::vector<size_t> triangleIndexes; // indices into the triangle array

    /* Construct a mesh object from a range of triangles. `l` and `u` inclusive. */
    MeshObject (size_t l, size_t u) : SceneObject()
    {
        triangleIndexes.reserve(u);
        for (size_t i = l; i <= u; i++)
        {
            triangleIndexes.push_back(i);
        }
    }

    static MeshObject fromTriangles(const std::vector<size_t>& triangleIndexes)
    {
        MeshObject meshObject = MeshObject(0, triangleIndexes.size() - 1);
        meshObject.triangleIndexes = triangleIndexes;
        return meshObject;
    }
};