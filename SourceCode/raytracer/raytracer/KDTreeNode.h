#pragma once
#include <vector>
#include <array>
#include <memory>
#include <string>

#include "json.hpp"
#include "AABB.h"

class Scene;
class TraceHit;
class Ray;
class KDTreeNode
{
    using ordered_json = nlohmann::ordered_json;
public:
    KDTreeNode() = default;
    KDTreeNode(const AABB& aabb) : aabb(aabb) {}
    KDTreeNode(const KDTreeNode&) = delete;
    KDTreeNode& operator=(const KDTreeNode&) = delete;
    KDTreeNode(KDTreeNode&&) = default;
    KDTreeNode& operator=(KDTreeNode&&) = default;

    ~KDTreeNode() = default;

    void build(std::vector<size_t>&& triangleRefs, const std::vector<AABB>& triangleAABBs, size_t maxTrianglesPerLeaf, size_t maxDepth, size_t depth = 0);

    void traverse(const Scene& scene, const Ray& ray, TraceHit& out) const;

    std::array<const KDTreeNode*, 2> sortChildrenClosest(const Ray& ray) const;

    ordered_json toJson() const;

    std::string toString() const;

    AABB aabb {};
private:
    std::vector<size_t> triangleRefs {};

    bool isLeaf() const { return child[0] == nullptr; }

    std::array<std::unique_ptr<KDTreeNode>, 2> child {nullptr, nullptr};

    int axisSplit = -1;
};

