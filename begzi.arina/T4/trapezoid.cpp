#include "trapezoid.h"
#include <algorithm>

IsoscelesTrapezoid::IsoscelesTrapezoid(Point p, double bb, double tb, double h) {
    bl = p;
    bBase = bb;
    tBase = tb;
    height = h;
}

double IsoscelesTrapezoid::getArea() const {
    return (bBase + tBase) / 2.0 * height;
}

Point IsoscelesTrapezoid::getCenter() const {
    double cx = bl.x + bBase / 2.0;
    double cy = bl.y + height / 2.0;
    return { cx, cy };
}

void IsoscelesTrapezoid::move(double dx, double dy) {
    bl.x = bl.x + dx;
    bl.y = bl.y + dy;
}

void IsoscelesTrapezoid::scale(double factor) {
    Point c = getCenter();
    bBase = bBase * factor;
    tBase = tBase * factor;
    height = height * factor;
    bl.x = c.x - bBase / 2.0;
    bl.y = c.y - height / 2.0;
}

std::string IsoscelesTrapezoid::getName() const {
    return "TRAPEZOID";
}

void IsoscelesTrapezoid::getBoundingBox(Point& minP, Point& maxP) const {
    double cx = bl.x + bBase / 2.0;
    double x1 = bl.x;
    double x2 = bl.x + bBase;
    double x3 = cx - tBase / 2.0;
    double x4 = cx + tBase / 2.0;

    minP.x = std::min({ x1, x2, x3, x4 });
    minP.y = bl.y;
    maxP.x = std::max({ x1, x2, x3, x4 });
    maxP.y = bl.y + height;
}
