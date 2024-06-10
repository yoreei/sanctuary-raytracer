#pragma once

void hw4()
{
    std::cout << "Task 2" << std::endl;
    Vec3 A = {3.5, 0, 0};
    Vec3 B = {1.75, 3.5, 0};
    std::cout << A.toString() << " cross " << B.toString() << " = " << A.cross(B).toString() <<std::endl;

    A = {3,-3,1};
    B = {4, 9, 2};
    std::cout << A.toString() << " cross " << B.toString() << " = " << A.cross(B).toString() <<std::endl;

    std::cout << A.toString() << " parallelogram " << B.toString() << " = " << A.cross(B).length() <<std::endl;

    A = {3,-3,1};
    B = {-12, 12, -4};
    std::cout << A.toString() << " parallelogram " << B.toString() << " = " << A.cross(B).length() <<std::endl;

    std::cout << std::endl << "Task 3" << std::endl;
    Triangle t = Triangle{{-1.75, -1.75, -3}, {1.75, -1.75, -3}, {0, 1.75, -3}};
    std::cout << t.toString() << " normal: " << t.normal().toString() << std::endl;
    std::cout << t.toString() << " area: " << t.area() << std::endl;

    t = Triangle{{0, 0, -1},{1, 0, 1},{-1, 0, 1}};
    std::cout << t.toString() << " normal: " << t.normal().toString() << std::endl;
    std::cout << t.toString() << " area: " << t.area() << std::endl;

    t = Triangle{{0.56, 1.11, 1.23},{0.44, -2.368, -0.54},{-1.56, 0.15, -1.92}};
    std::cout << t.toString() << " normal: " << t.normal().toString() << std::endl;
    std::cout << t.toString() << " area: " << t.area() << std::endl;

}