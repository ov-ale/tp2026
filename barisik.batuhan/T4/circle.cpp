#define _USE_MATH_DEFINES
#include "circle.h"
#include <cmath>
#include <stdexcept>

Circle::Circle(const Point& c, double r) : center(c), radius(r) {
    if (r <= 0) {
        throw std::invalid_argument("Error: radius must be positive");
    }
}
double Circle::getArea() const {
    return M_PI * radius * radius;
}
Point Circle::getCenter() const {
    return center;
}
void Circle::move(double dx, double dy) {
    center.x += dx;
    center.y += dy;
}
void Circle::scale(double factor) {
    if (factor <= 0) {
        throw std::invalid_argument("scale factor must be positive");
    }
    radius *= factor;
}
std::string Circle::getName() const {
    return "CIRCLE";
}
bool Circle::containsPoint(const Point& p) const {
    double dx = p.x - center.x;
    double dy = p.y - center.y;
    return (dx * dx + dy * dy) <= (radius * radius);
}
double Circle::getPerimeter() const {
    return 2 * M_PI * radius;
}
void Circle::getBoundingBox(Point& min, Point& max) const {
    min = Point(center.x - radius, center.y - radius);
    max = Point(center.x + radius, center.y + radius);
}
//
