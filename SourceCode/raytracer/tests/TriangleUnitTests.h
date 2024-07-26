#pragma once

#include <cassert>

#include "Triangle.h"
#include "CRTTypes.h"
#include "Scene.h"
#include "TestUtils.h"

namespace TriangleUnitTests
{
    using namespace std;

    void run()
    {
        size_t materialIdx = 0;
        Settings settings {};
        Scene scene{"test1", &settings};
        float area;
        auto& vs = scene.vertices;
        Vec3 n;
        Triangle& t = addTriangle(scene, { -1.75f, -1.75f, -3.f }, { 1.75f, -1.75f, -3.f }, { 0.f, 1.75f, -3.f });
        n = t.getNormal();

        assert(n.equal({ 0.f, 0.f, 1.f }));
        area = t.area(vs);
        assert(fEqual(area, 6.125f));

        resetScene(scene);
        t = addTriangle(scene, { 0.f, 0.f, -1.f }, { 1.f, 0.f, 1.f }, { -1.f, 0.f, 1.f });
        n = t.getNormal();
        assert(n.equal({ 0.f, -1.f, 0.f }));
        assert(fEqual(t.area(vs), 2.f));

        resetScene(scene);
        t = addTriangle(scene, { 0.56f, 1.11f, 1.23f }, { 0.44f, -2.368f, -0.54f }, { -1.56f, 0.15f, -1.92f });
        n = t.getNormal();
        assert(n.equal({ 0.756420f, 0.275748f, -0.593120f }));
        assert(fEqual(t.area(vs), 6.11862f));

        // intersect
        t = addTriangle(scene, { -2.f, 1.5f, 0.f }, { 0.f, 1.5f, -5.f }, { 2.f, 1.5f, 0.f });
        scene.uvs.resize(scene.vertices.size(), Vec3{0.f, 0.f, 0.f});
        Ray ray { {0.f, 0.f, -1.f}, {0.f, 1.f, 0.f} };
        TraceHit hit {};
        scene.materials.push_back(Material{});
        Material& material = scene.materials.back();
        material.type = Material::Type::DIFFUSE;
        t.intersect(scene, ray, hit);
        assert(hit.successful());

    }
} // namespace TriangleUnitTests
