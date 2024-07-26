#pragma once
#include "CRTTypes.h"
#include "KDTreeNode.h"

namespace KDTreeUnitTests {

    void run() {
        AABB rootAabb{ {-6.f, -6.f, -6.f}, {6.f, 6.f, 6.f} };
        KDTreeNode root{ rootAabb };
        root.axisSplit
            zzzzzzz

        AABB child0Aabb{ {-6.f, -6.f, -6.f}, {6.f, 6.f, 0.f} };
        root.child[0] = std::make_unique<KDTreeNode>(child0Aabb);

        AABB child1Aabb{ {6.f, 6.f, 0.f}, {6.f, 6.f, 6.f} };
        root.child[1] = std::make_unique<KDTreeNode>(child1Aabb);

        auto sorted = root.closestChildren({ 0.f, 0.f, -8.f });
        const KDTreeNode* closest = sorted[0];
        const KDTreeNode* next = sorted[1];

        assert(closest->aabb.bounds[1].equal({6.f, 6.f, 0.f}));
        assert(next->aabb.bounds[1].equal({6.f, 6.f, 6.f}));

    }
}
