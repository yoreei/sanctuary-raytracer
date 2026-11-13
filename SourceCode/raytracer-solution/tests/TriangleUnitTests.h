#pragma once

#include "include/CRTTypes.h"
#include "include/Scene.h"
#include "include/TraceHit.h"
#include "include/Triangle.h"
#include "TestUtils.h"

namespace TriangleUnitTests
{
    using namespace TestUtils;

    inline void normalAndArea()
    {
        Settings settings{};

        Scene s1 = makeScene("tri1", settings);
        addTriangle(s1, { -1.75f, -1.75f, -3.f }, { 1.75f, -1.75f, -3.f }, { 0.f, 1.75f, -3.f });
        buildScene(s1);
        EXPECT(s1.triangles[0].getNormal().equal({ 0.f, 0.f, 1.f }));
        EXPECT(fEqual(s1.triangles[0].area(s1.vertices), 6.125f));

        Scene s2 = makeScene("tri2", settings);
        addTriangle(s2, { 0.f, 0.f, -1.f }, { 1.f, 0.f, 1.f }, { -1.f, 0.f, 1.f });
        buildScene(s2);
        EXPECT(s2.triangles[0].getNormal().equal({ 0.f, -1.f, 0.f }));
        EXPECT(fEqual(s2.triangles[0].area(s2.vertices), 2.f));

        Scene s3 = makeScene("tri3", settings);
        addTriangle(s3, { 0.56f, 1.11f, 1.23f }, { 0.44f, -2.368f, -0.54f }, { -1.56f, 0.15f, -1.92f });
        buildScene(s3);
        EXPECT(s3.triangles[0].getNormal().equal({ 0.756420f, 0.275748f, -0.593120f }));
        EXPECT(fEqual(s3.triangles[0].area(s3.vertices), 6.11862f));
    }

    inline void intersection()
    {
        Settings settings{};
        Scene scene = makeScene("intersect", settings);
        // Triangle in the plane y = 1.5 with its normal pointing down (-y)
        addTriangle(scene, { -2.f, 1.5f, 0.f }, { 0.f, 1.5f, -5.f }, { 2.f, 1.5f, 0.f });
        buildScene(scene);
        const Triangle& tri = scene.triangles[0];
        EXPECT(tri.getNormal().equal({ 0.f, -1.f, 0.f }));

        // Front face: ray going up hits at y = 1.5
        TraceHit hit{};
        tri.intersect(scene, Ray{ { 0.f, 0.f, -1.f }, { 0.f, 1.f, 0.f } }, 0, hit);
        EXPECT(hit.type == TraceHitType::SUCCESS);
        EXPECT(fEqual(hit.t, 1.5f));
        EXPECT(hit.p.equal({ 0.f, 1.5f, -1.f }));

        // Plane behind the ray origin
        TraceHit behind{};
        tri.intersect(scene, Ray{ { 0.f, 0.f, -1.f }, { 0.f, -1.f, 0.f } }, 0, behind);
        EXPECT(!behind.successful());

        // Back face of a diffuse triangle does not count as a hit
        TraceHit backface{};
        tri.intersect(scene, Ray{ { 0.f, 3.f, -1.f }, { 0.f, -1.f, 0.f } }, 0, backface);
        EXPECT(backface.type == TraceHitType::PLANE_BACKFACE);

        // Front face, but outside the triangle
        TraceHit outside{};
        tri.intersect(scene, Ray{ { 0.f, 0.f, -1.f }, Vec3{ 0.9f, 0.3f, 0.f }.getUnit() }, 0, outside);
        EXPECT(!outside.successful());
    }

    inline void run()
    {
        normalAndArea();
        intersection();
    }
} // namespace TriangleUnitTests
