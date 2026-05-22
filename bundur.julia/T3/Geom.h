#ifndef GEOM_H
#define GEOM_H
#include <vector>
#include <istream>
struct Point {
    int x, y;
};
struct Polygon {
    std::vector<Point> points;
};
std::istream& operator>>(std::istream& is, Point& pt);
std::istream& operator>>(std::istream& is, Polygon& poly);
bool operator==(const Point& lhs, const Point& rhs);
bool operator==(const Polygon& lhs, const Polygon& rhs);
double getPolygonArea(const Polygon& poly);
bool isRectangle(const Polygon& poly);
#endif
