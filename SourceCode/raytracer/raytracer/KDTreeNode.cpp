#include "KDTreeNode.h"

#include "json.hpp"

#include "TraceHit.h"
#include "CRTTypes.h"
#include "Scene.h"

using ordered_json = nlohmann::ordered_json;

void KDTreeNode::traverse(const Scene& scene, const Ray& ray, TraceHit& out) const {
    out.t = std::numeric_limits<float>::max();
    out.type = TraceHitType::OUT_OF_BOUNDS;

    traverseRecursive(scene, ray, out);
}

void KDTreeNode::traverseRecursive(const Scene& scene, const Ray& ray, TraceHit& out) const {
    // Early out
    if (!aabb.hasIntersection(ray)) {
        // out.type = TraceHitType::AABB_PRUNE; wrong, overrides success!
        return;
    }

    if (isLeaf()) {
        for (const size_t& triRef : triangleRefs) {
            const Triangle& tri = scene.triangles[triRef];
            if (!scene.triangleAABBs[triRef].hasIntersection(ray)) continue;

            TraceHit tryHit{};
            tri.intersect(scene, ray, tryHit);
            if (tryHit.successful() && tryHit.t < out.t && aabb.contains(tryHit.p)) {
                out = tryHit;
            }
        }
    }
    else {
        std::array<const KDTreeNode*, 2> sortedChildren = closestChildren(ray.origin);

        for (size_t i = 0; i < sortedChildren.size(); ++i) {
            const KDTreeNode* childPtr = sortedChildren[i];
            childPtr->traverseRecursive(scene, ray, out);
            processAabbDebug(scene, i, out);

            if (out.successful()) {
                break;
            }
        }
    }
}

void KDTreeNode::processAabbDebug(const Scene& scene, size_t childIdx, TraceHit& out) const
{
    if (out.type != TraceHitType::AABB_PRUNE && scene.settings->showAabbs) {
        Vec3 traceDebugColor = { 0.f, 0.f, 0.f };
        traceDebugColor.axis(childIdx % 3) += 0.3f;
        out.traceColor += traceDebugColor;
        std::cout << "traversing child: " << childIdx << std::endl;
    }
}

std::array<const KDTreeNode*, 2> KDTreeNode::closestChildren(const Vec3& point) const {
    assert(axisSplit >= 0 && axisSplit <= 2);

    float dist0 = child[0]->aabb.distanceToAxis(axisSplit, point);
    float dist1 = child[1]->aabb.distanceToAxis(axisSplit, point);
    if (dist0 < dist1) {
        return { child[0].get(), child[1].get() };
    }
    else {
        return { child[1].get(), child[0].get() };
    }
}

void KDTreeNode::build(std::vector<size_t>&& newTriangleRefs, const std::vector<AABB>& triangleAABBs, size_t maxTrianglesPerLeaf, size_t maxDepth, size_t depth)
{
    // Required: newTriangleRefs intersect aabb

    // 0.1 Recursion Root (Make Leaf)
    if (depth >= maxDepth /* || newTriangleRefs.size() <= maxTrianglesPerLeaf */) {
        triangleRefs = std::move(newTriangleRefs);
        return; // Leaf
    }

    // 0.2 Recursion Root 2. Stop if AABB too small. Check only X axis for performance reasons.
    float size = aabb.bounds[1].x - aabb.bounds[0].x;
    if (size <= 1.0E-4F) {
        triangleRefs = std::move(newTriangleRefs);
        return; // Leaf
    }

    // 1. Split Triangles in 2 Groups (Potential Children Nodes)
    // 1.1 Axis Splitting And Compute axisSplit
    axisSplit = int(aabb.getMaxAxis());
    float splitValue = (aabb.bounds[0].axis(axisSplit) + aabb.bounds[1].axis(axisSplit)) / 2;

    std::vector<size_t> triangleRefs0 {};
    AABB child0Aabb = aabb;
    child0Aabb.bounds[1].axis(axisSplit) = splitValue;

    std::vector<size_t> triangleRefs1 {};
    AABB child1Aabb = aabb;
    child1Aabb.bounds[0].axis(axisSplit) = splitValue;

    for (const auto& candidateRef : newTriangleRefs) {
        int dbgPushBacks = 0;
        if (child0Aabb.hasIntersection(triangleAABBs[candidateRef])) {
            triangleRefs0.push_back(candidateRef);
            ++dbgPushBacks;
        }
        if (child1Aabb.hasIntersection(triangleAABBs[candidateRef])) {
            triangleRefs1.push_back(candidateRef);
            ++dbgPushBacks;
        }
        assert(dbgPushBacks > 0);
    }
    assert(triangleRefs0.size() + triangleRefs1.size() >= newTriangleRefs.size());

    // 3. Else, Create Children
    child[0] = std::make_unique<KDTreeNode>(child0Aabb, 0);
    child[1] = std::make_unique<KDTreeNode>(child1Aabb, 1);
    child[0]->build(std::move(triangleRefs0), triangleAABBs, maxTrianglesPerLeaf, maxDepth, depth + 1);
    child[1]->build(std::move(triangleRefs1), triangleAABBs, maxTrianglesPerLeaf, maxDepth, depth + 1);
}

ordered_json KDTreeNode::toJson() const {
    ordered_json j{};

    j["min"] = { aabb.bounds[0].x, aabb.bounds[0].y, aabb.bounds[0].z };
    j["max"] = { aabb.bounds[1].x, aabb.bounds[1].y, aabb.bounds[1].z };
    j["triangleRefs"] = triangleRefs;

    j["child0"] = child[0] ? child[0]->toJson() : nullptr;
    j["child1"] = child[1] ? child[1]->toJson() : nullptr;
    return j;
}

std::string KDTreeNode::toString() const
{
    return toJson().dump(4);
}
