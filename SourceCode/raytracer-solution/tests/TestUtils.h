#pragma once
#include <iostream>
#include <streambuf>
#include <vector>

#include "include/CRTTypes.h"
#include "include/Material.h"
#include "include/Scene.h"
#include "include/Triangle.h"

/* Unlike assert(), EXPECT also runs in Release builds and does not stop at the first failure. */
#define EXPECT(expr) ::TestUtils::check(static_cast<bool>(expr), #expr, __FILE__, __LINE__)

namespace TestUtils
{
    inline size_t& checkCount() { static size_t n = 0; return n; }
    inline size_t& failureCount() { static size_t n = 0; return n; }

    inline void check(bool passed, const char* expr, const char* file, int line)
    {
        ++checkCount();
        if (!passed) {
            ++failureCount();
            std::cerr << file << "(" << line << "): CHECK failed: " << expr << "\n";
        }
    }

    /* Discards std::cout while alive. The library logs heavily to std::cout
       (Scene::build dumps the KD-tree, Debug traversal logs every node). */
    class MuteCout {
    public:
        MuteCout() : saved(std::cout.rdbuf(nullptr)) {}
        ~MuteCout() { std::cout.rdbuf(saved); }
        MuteCout(const MuteCout&) = delete;
        MuteCout& operator=(const MuteCout&) = delete;
    private:
        std::streambuf* saved;
    };

    /* Scene with a single default (diffuse) material at index 0 */
    inline Scene makeScene(const std::string& name, const Settings& settings)
    {
        Scene scene{ name, &settings };
        scene.materials.push_back(Material{});
        return scene;
    }

    /* Adds one triangle as its own mesh object. Call `buildScene` before intersecting. */
    inline void addTriangle(Scene& scene, const Vec3& v0, const Vec3& v1, const Vec3& v2)
    {
        std::vector<Vec3> vertices{ v0, v1, v2 };
        std::vector<Triangle> triangles{ Triangle{ 0, 1, 2, 0 } };
        std::vector<Vec3> uvs(vertices.size(), Vec3{ 0.f, 0.f, 0.f });
        scene.addObject(vertices, triangles, uvs);
    }

    inline void buildScene(Scene& scene)
    {
        MuteCout mute;
        scene.build();
    }
} // namespace TestUtils
