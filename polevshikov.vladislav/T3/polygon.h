#ifndef POLYGON_H
#define POLYGON_H

#include <iostream>
#include <vector>

struct Point {
    int x, y;
};

struct Polygon {
    std::vector<Point> points;
};

std::istream& operator>>(std::istream& in, Polygon& dest);

double getArea(const Polygon& poly);
bool hasRightAngle(const Polygon& poly);

#endif
