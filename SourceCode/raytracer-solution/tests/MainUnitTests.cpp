#include <iostream>

#include "include/CRTTypesUnitTests.h"
#include "include/TriangleUnitTests.h"
#include "include/CameraUnitTests.h"
#include "include/Benchmarks.h"
#include "include/SceneUnitTests.h"
#include "include/KDTreeUnitTests.h"
//#include "include/RendererIntegrationTests.h"

int main()
{
    CRTTypesUnitTests::run();
    TriangleUnitTests::run();
    CameraUnitTests::run();
    SceneUnitTests::run();
    KDTreeUnitTests::run();
    Benchmarks::run();

    //RendererIntegrationTests::run();

    std::cout<<"All tests passed!"<<std::endl;
    return 0;
}