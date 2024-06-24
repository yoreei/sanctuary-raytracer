#pragma once

#include <vector>

#include "SceneObject.h"

class MeshObject : public SceneObject
{
public:
    std::vector<size_t> triangles; // indices into the triangle array

    /* Construct a mesh object from a range of triangles. `l` and `u` inclusive. */
    MeshObject (size_t l, size_t u) : SceneObject()
    {
        triangles.reserve(u);
        for (size_t i = l; i <= u; i++)
        {
            triangles.push_back(i);
        }
    }

    static MeshObject fromTriangles(const std::vector<size_t>& triangles)
    {
        MeshObject meshObject = MeshObject(0, triangles.size() - 1);
        meshObject.triangles = triangles;
        return meshObject;
    }
};