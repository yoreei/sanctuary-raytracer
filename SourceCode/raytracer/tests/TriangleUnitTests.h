#pragma once

#include <cassert>

#include "Triangle.h"
#include "CRTTypes.h"
#include "Scene.h"
#include "TestUtils.h"

namespace TriangleUnitTests
{
    Triangle createTriangle(std::vector<Vec3>& vs, const Vec3& v0, const Vec3& v1, const Vec3& v2)
    {
        vs.resize(3);
        vs[0] = v0;
        vs[1] = v1;
        vs[2] = v2;
        return Triangle{ {v0, v1, v2}, 0, 1, 2, 0 };
    }

    using namespace std;

    void run()
    {
        size_t materialIdx = 0;
        Scene scene{"test"};
        float area;
        auto& vs = scene.vertices;
        Vec3 n;
        Triangle t = createTriangle(vs, { -1.75f, -1.75f, -3.f }, { 1.75f, -1.75f, -3.f }, { 0.f, 1.75f, -3.f });
        n = t.getNormal();

        assert(n.equal({ 0.f, 0.f, 1.f }));
        area = t.area(vs);
        assert(fequal(area, 6.125f));

        t = createTriangle(vs, { 0.f, 0.f, -1.f }, { 1.f, 0.f, 1.f }, { -1.f, 0.f, 1.f });
        n = t.getNormal();

        assert(n.equal({ 0.f, -1.f, 0.f }));
        assert(fequal(t.area(vs), 2.f));

        t = createTriangle(vs, { 0.56f, 1.11f, 1.23f }, { 0.44f, -2.368f, -0.54f }, { -1.56f, 0.15f, -1.92f });
        n = t.getNormal();

        assert(n.equal({ 0.756420f, 0.275748f, -0.593120f, }));
        assert(fequal(t.area(vs), 6.11862f));

        // intersect
        t = createTriangle(vs, { -2.f, 1.5f, 0.f }, { 0.f, 1.5f, -5.f }, { 2.f, 1.5f, 0.f });
        Ray ray { {0.f, 0.f, -1.f}, {0.f, 1.f, 0.f} };
        TraceHit hit {};
        scene.materials.push_back(Material{});
        Material& material = scene.materials.back();
        material.type = Material::Type::DIFFUSE;
        t.intersect(scene, ray, hit);
        assert(hit.successful());

    }
} // namespace TriangleUnitTests
