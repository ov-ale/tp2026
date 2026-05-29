#include "isosceles_trapezoid.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>

IsoscelesTrapezoid::IsoscelesTrapezoid(const Point& bl, double bottomBase,
                                       double topBase, double height)
    : bottomLeft(bl), bottomBase(bottomBase), topBase(topBase), height(height) {
    if (bottomBase <= 0.0 || topBase <= 0.0 || height <= 0.0) {
        throw std::invalid_argument(
            "Error: bases and height must be positive");
    }
    if (topBase > bottomBase) {
        throw std::invalid_argument(
            "Error: top base cannot be larger than bottom base");
    }
}
double IsoscelesTrapezoid::getArea() const {
    return (bottomBase + topBase) / 2.0 * height;
}
Point IsoscelesTrapezoid::getCenter() const {
    double cx = bottomLeft.x + (bottomBase + topBase) / 4.0;
    double cy = bottomLeft.y + height / 2.0;
    return Point(cx, cy);
}
void IsoscelesTrapezoid::move(double dx, double dy) {
    bottomLeft.x += dx;
    bottomLeft.y += dy;
}
void IsoscelesTrapezoid::scale(double factor) {
    if (factor <= 0.0) {
        throw std::invalid_argument("scale factor must be positive");
    }
    Point center = getCenter();
    double xBr = bottomLeft.x + bottomBase;
    double offset = (bottomBase - topBase) / 2.0;
    double xTl = bottomLeft.x + offset;
    double xTr = xTl + topBase;
    double yTop = bottomLeft.y + height;

    bottomLeft.x = center.x + (bottomLeft.x - center.x) * factor;
    bottomLeft.y = center.y + (bottomLeft.y - center.y) * factor;
    xBr = center.x + (xBr - center.x) * factor;
    xTl = center.x + (xTl - center.x) * factor;
    xTr = center.x + (xTr - center.x) * factor;
    yTop = center.y + (yTop - center.y) * factor;

    bottomBase = std::abs(xBr - bottomLeft.x);
    topBase = std::abs(xTr - xTl);
    height = std::abs(yTop - bottomLeft.y);
}
std::string IsoscelesTrapezoid::getName() const {
    return "ISOSCELES_TRAPEZOID";
}
bool IsoscelesTrapezoid::containsPoint(const Point& p) const {
    if (p.y < bottomLeft.y || p.y > bottomLeft.y + height) {
        return false;
    }
    double t = (p.y - bottomLeft.y) / height;
    double widthAtY = bottomBase * (1 - t) + topBase * t;
    double leftX = bottomLeft.x + (bottomBase - widthAtY) / 2.0;
    return (p.x >= leftX && p.x <= leftX + widthAtY);
}
double IsoscelesTrapezoid::getPerimeter() const {
    double sideOffset = (bottomBase - topBase) / 2.0;
    double side = std::sqrt(sideOffset * sideOffset + height * height);
    return bottomBase + topBase + 2.0 * side;
}
void IsoscelesTrapezoid::getBoundingBox(Point& min, Point& max) const {
    double centerX = bottomLeft.x + bottomBase / 2.0;
    double x1 = bottomLeft.x;
    double x2 = bottomLeft.x + bottomBase;
    double x3 = centerX - topBase / 2.0;
    double x4 = centerX + topBase / 2.0;
    min.x = std::min({x1, x2, x3, x4});
    max.x = std::max({x1, x2, x3, x4});
    min.y = bottomLeft.y;
    max.y = bottomLeft.y + height;
}
//

