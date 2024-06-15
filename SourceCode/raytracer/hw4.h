#pragma once

void hw4()
{
    using namespace std;
    cout << "Task 2" << endl;
    Vec3 A = {3.5, 0, 0};
    Vec3 B = {1.75, 3.5, 0};
    cout << A.toString() << " cross " << B.toString() << " = " << A.cross(B).toString() << endl;

    A = {3,-3,1};
    B = {4, 9, 2};
    cout << A.toString() << " cross " << B.toString() << " = " << A.cross(B).toString() << endl;

    cout << A.toString() << " parallelogram " << B.toString() << " = " << A.cross(B).length() << endl;

    A = {3,-3,1};
    B = {-12, 12, -4};
    cout << A.toString() << " parallelogram " << B.toString() << " = " << A.cross(B).length() << endl;

    cout << endl << "Task 3" << endl;
    Triangle t = Triangle{{-1.75, -1.75, -3}, {1.75, -1.75, -3}, {0, 1.75, -3}};
    cout << t.toString() << " normal: " << t.normal().toString() << endl;
    cout << t.toString() << " area: " << t.area() << endl;

    t = Triangle{{0, 0, -1},{1, 0, 1},{-1, 0, 1}};
    cout << t.toString() << " normal: " << t.normal().toString() << endl;
    cout << t.toString() << " area: " << t.area() << endl;

    t = Triangle{{0.56, 1.11, 1.23},{0.44, -2.368, -0.54},{-1.56, 0.15, -1.92}};
    cout << t.toString() << " normal: " << t.normal().toString() << endl;
    cout << t.toString() << " area: " << t.area() << endl;

    /*
Output:

Task 2
3.500000 0.000000 0.000000 cross 1.750000 3.500000 0.000000 = 0.000000 0.000000 12.250000
3.000000 -3.000000 1.000000 cross 4.000000 9.000000 2.000000 = -15.000000 -2.000000 39.000000
3.000000 -3.000000 1.000000 parallelogram 4.000000 9.000000 2.000000 = 41.833
3.000000 -3.000000 1.000000 parallelogram -12.000000 12.000000 -4.000000 = 0

Task 3
Triangle: {-1.750000 -1.750000 -3.000000, 1.750000 -1.750000 -3.000000, 0.000000 1.750000 -3.000000} normal: 0.000000 0.000000 1.000000
Triangle: {-1.750000 -1.750000 -3.000000, 1.750000 -1.750000 -3.000000, 0.000000 1.750000 -3.000000} area: 6.125
Triangle: {0.000000 0.000000 -1.000000, 1.000000 0.000000 1.000000, -1.000000 0.000000 1.000000} normal: 0.000000 -1.000000 0.000000
Triangle: {0.000000 0.000000 -1.000000, 1.000000 0.000000 1.000000, -1.000000 0.000000 1.000000} area: 2
Triangle: {0.560000 1.110000 1.230000, 0.440000 -2.368000 -0.540000, -1.560000 0.150000 -1.920000} normal: 0.756420 0.275748 -0.593120
Triangle: {0.560000 1.110000 1.230000, 0.440000 -2.368000 -0.540000, -1.560000 0.150000 -1.920000} area: 6.11862

*/

}
