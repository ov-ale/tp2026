#ifndef POINT_H
#define POINT_H

struct Point {
    double x;
    double y;

    Point() = default;
    Point(double x, double y) : x(x), y(y) {}
};

#endif
