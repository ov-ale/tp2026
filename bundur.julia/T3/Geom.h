#ifndef GEOM_H
#define GEOM_H
#include <vector>
#include <iostream>
struct Point {
    int x, y;
    bool operator==(const Point& other) const;
};
struct Polygon {
    std::vector<Point> points;
    bool operator==(const Polygon& other) const;
};
std::istream& operator>>(std::istream& in, Polygon& p);
double getPolygonArea(const Polygon& p);
bool isRectangle(const Polygon& p);
#endif
