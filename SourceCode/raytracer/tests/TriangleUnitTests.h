#pragma once

#include <cassert>

#include "Triangle.h"
#include "CRTTypes.h"
#include "Scene.h"

namespace TriangleUnitTests
{
    using namespace std;
    void run()
    {
        size_t materialIdx = 0;
        Scene scene{};
        Triangle t = Triangle{ 0, 1, 2, materialIdx };
        Vec3 n;
        float area;
        auto& vs = scene.vertices;
        vs = { { -1.75f, -1.75f, -3.f }, { 1.75f, -1.75f, -3.f }, { 0.f, 1.75f, -3.f } };

        t.normal(vs, n);

        assert(n.equal({ 0.f, 0.f, 1.f }));
        area = t.area(vs);
        assert(fequal(area, 6.125f));

        vs = { {0.f, 0.f, -1.f},{1.f, 0.f, 1.f},{-1.f, 0.f, 1.f} };
        t.normal(vs, n);

        assert(n.equal({ 0.f, -1.f, 0.f }));
        assert(fequal(t.area(vs), 2.f));

        vs = { {0.56f, 1.11f, 1.23f},{0.44f, -2.368f, -0.54f},{-1.56f, 0.15f, -1.92f} };
        t.normal(vs, n);

        assert(n.equal({ 0.756420f, 0.275748f, -0.593120f, }));
        assert(fequal(t.area(vs), 6.11862f));
    }
} // namespace TriangleUnitTests
