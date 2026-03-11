#include "circle.h"
#include <cmath>

Circle::Circle(const Point& c, double r) {
    center = c;
    radius = r;
}

double Circle::getArea() const {
    return acos(-1.0) * radius * radius;
}

Point Circle::getCenter() const {
    return center;
}

void Circle::move(double dx, double dy) {
    center.x_ += dx;
    center.y_ += dy;
}

void Circle::scale(double factor) {
    radius *= factor;
}

std::string Circle::getName() const {
    return "CIRCLE";
}
