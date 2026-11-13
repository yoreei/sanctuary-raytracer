#pragma once
#include <cfloat>
#include <iostream>
#include <random>

#include "include/CRTTypes.h"
#include "include/Scene.h"
#include "include/TraceHit.h"
#include "TestUtils.h"

namespace KDTreeUnitTests
{
    using namespace TestUtils;

    /* Random "triangle soup": small triangles scattered in [-5, 5]^3 with random winding */
    inline void addRandomTriangles(Scene& scene, std::mt19937& rng, size_t count)
    {
        std::uniform_real_distribution<float> center(-5.f, 5.f);
        std::uniform_real_distribution<float> offset(-0.6f, 0.6f);

        std::vector<Vec3> vertices{};
        std::vector<Triangle> triangles{};
        for (size_t i = 0; i < count; ++i) {
            Vec3 c{ center(rng), center(rng), center(rng) };
            for (int k = 0; k < 3; ++k) {
                vertices.push_back(c + Vec3{ offset(rng), offset(rng), offset(rng) });
            }
            triangles.emplace_back(3 * i, 3 * i + 1, 3 * i + 2, 0);
        }
        std::vector<Vec3> uvs(vertices.size(), Vec3{ 0.f, 0.f, 0.f });
        scene.addObject(vertices, triangles, uvs);
    }

    inline Ray randomRay(std::mt19937& rng)
    {
        std::uniform_real_distribution<float> origin(-8.f, 8.f);
        std::normal_distribution<float> dir(0.f, 1.f);
        Vec3 d{ dir(rng), dir(rng), dir(rng) };
        return Ray{ { origin(rng), origin(rng), origin(rng) }, d.getUnit() };
    }

    /* Reference: test every triangle, keep the closest successful hit */
    inline TraceHit bruteForce(const Scene& scene, const Ray& ray)
    {
        TraceHit best{};
        for (size_t i = 0; i < scene.triangles.size(); ++i) {
            TraceHit hit{};
            scene.triangles[i].intersect(scene, ray, i, hit);
            if (hit.successful() && hit.t < best.t) {
                best = hit;
            }
        }
        return best;
    }

    /* The KD-tree must find exactly the hits brute force finds, for any tree shape.
       Depth is capped: KDTreeNode::build never stops on overlapping triangles otherwise. */
    inline void matchesBruteForce()
    {
        struct TreeShape { size_t maxTrianglesPerLeaf; size_t maxDepth; };
        const TreeShape shapes[] = { {4, 16}, {1, 16}, {8, 3}, {4, 1}, {4, 0} };

        for (const TreeShape& shape : shapes) {
            Settings settings{};
            settings.maxTrianglesPerLeaf = shape.maxTrianglesPerLeaf;
            settings.accelTreeMaxDepth = shape.maxDepth;

            std::mt19937 rng{ 1234 };
            Scene scene = makeScene("kdtree", settings);
            addRandomTriangles(scene, rng, 300);
            buildScene(scene);

            size_t hits = 0;
            size_t mismatches = 0;
            for (int i = 0; i < 3000; ++i) {
                Ray ray = randomRay(rng);
                TraceHit expected = bruteForce(scene, ray);
                TraceHit actual{};
                {
                    MuteCout mute; // Debug traversal logs every visited node
                    scene.intersect(ray, actual);
                }

                bool same = expected.successful() == actual.successful();
                if (same && expected.successful()) {
                    ++hits;
                    same = actual.triRef == expected.triRef || fEqual(actual.t, expected.t, 1e-4f);
                }
                if (!same) {
                    ++mismatches;
                }
            }

            if (mismatches > 0) {
                std::cerr << "KD-tree (maxTrianglesPerLeaf=" << shape.maxTrianglesPerLeaf
                    << ", maxDepth=" << shape.maxDepth << "): " << mismatches
                    << " of 3000 rays differ from brute force\n";
            }
            EXPECT(hits > 100); // the random scene must actually be hit
            EXPECT(mismatches == 0);
        }
    }

    inline void run()
    {
        matchesBruteForce();
    }
} // namespace KDTreeUnitTests
