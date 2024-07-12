#include <iostream>

#include "CRTTypesUnitTests.h"
#include "TriangleUnitTests.h"
#include "CameraUnitTests.h"
#include "RendererIntegrationTests.h"

int main()
{
    CRTTypesUnitTests::run();
    TriangleUnitTests::run();
    CameraUnitTests::run();

    RendererIntegrationTests::run();

    std::cout<<"All tests passed!"<<std::endl;
    return 0;
}