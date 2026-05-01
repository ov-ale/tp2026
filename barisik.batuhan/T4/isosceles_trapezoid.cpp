#include "isosceles_trapezoid.h"
#include <cmath>
#include <stdexcept>

IsoscelesTrapezoid::IsoscelesTrapezoid(const Point& bl, double bottomBase,
                                       double topBase, double height)
    : bottomLeft(bl), bottomBase(bottomBase), topBase(topBase), height(height) {
    if (bottomBase <= 0 || topBase <= 0 || height <= 0) {
        throw std::invalid_argument("Error: bases and height must be positive");
    }
    if (topBase > bottomBase) {
        throw std::invalid_argument(
            "Error: top base cannot be larger than bottom base");
    }
}
double IsoscelesTrapezoid::getArea() const {
    return (bottomBase + topBase) * height / 2.0;
}
Point IsoscelesTrapezoid::getCenter() const {
    double centerX = bottomLeft.x + bottomBase / 2.0;
    double centerY = bottomLeft.y + height / 2.0;
    return Point(centerX, centerY);
}
void IsoscelesTrapezoid::move(double dx, double dy) {
    bottomLeft.x += dx;
    bottomLeft.y += dy;
}
void IsoscelesTrapezoid::scale(double factor) {
    if (factor <= 0) {
        throw std::invalid_argument("scale factor must be positive");
    }
    Point center = getCenter();
    bottomLeft.x = center.x + (bottomLeft.x - center.x) * factor;
    bottomLeft.y = center.y + (bottomLeft.y - center.y) * factor;
    bottomBase *= factor;
    topBase *= factor;
    height *= factor;
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
    double rightX = leftX + widthAtY;
    return (p.x >= leftX && p.x <= rightX);
}
double IsoscelesTrapezoid::getPerimeter() const {
    double sideOffset = (bottomBase - topBase) / 2.0;
    double side = std::sqrt(sideOffset * sideOffset + height * height);
    return bottomBase + topBase + 2 * side;
}
void IsoscelesTrapezoid::getBoundingBox(Point& min, Point& max) const {
    min = bottomLeft;
    max = Point(bottomLeft.x + bottomBase, bottomLeft.y + height);
}