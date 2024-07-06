#include "Scene.h"
#include "Intersection.h"

bool Scene::intersect(const Ray& ray, IntersectionData& out) const {
    bool result = false;
    out.t = std::numeric_limits<float>::max();
    IntersectionData xData {};
    for (const Triangle& tri : triangles) {
        // TODO: Separate plane intersection & triangle uv intersection tests for perf.
        Intersection x = tri.intersect(vertices, ray, xData);
        if (xData.t < out.t && x == Intersection::SUCCESS) {
            out = xData;
            result = true;
        }

        metrics.record(toString(x));
    }
    return result;
}
