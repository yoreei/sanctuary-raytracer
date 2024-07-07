#include "Scene.h"
#include "Intersection.h"

/**
* Determine best intersection of ray with scene.
*/
void Scene::intersect(const Ray& ray, Intersection& out) const {
    out.t = std::numeric_limits<float>::max();
    Intersection xData {};
    for (const Triangle& tri : triangles) {
        // TODO: Separate plane intersection & triangle uv intersection tests?
        tri.intersect(*this, ray, xData);
        if (xData.successful() && xData.t < out.t) {
            out = xData;
        }

        metrics.record(toString(xData.type));
    }
}
