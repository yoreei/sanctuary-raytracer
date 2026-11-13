#include <iostream>

#include "TestUtils.h"
#include "CRTTypesUnitTests.h"
#include "CameraUnitTests.h"
#include "TriangleUnitTests.h"
#include "KDTreeUnitTests.h"

int main()
{
    // Triangle::intersect records per-thread metrics; the renderer reserves a slot per worker thread
    GSceneMetrics.reserveThread(1);

    CRTTypesUnitTests::run();
    CameraUnitTests::run();
    TriangleUnitTests::run();
    KDTreeUnitTests::run();

    const size_t checks = TestUtils::checkCount();
    const size_t failures = TestUtils::failureCount();
    if (failures > 0) {
        std::cout << failures << " of " << checks << " checks FAILED" << std::endl;
        return 1;
    }
    std::cout << "All " << checks << " checks passed" << std::endl;
    return 0;
}
