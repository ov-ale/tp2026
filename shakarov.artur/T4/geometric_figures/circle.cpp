#include "circle.h"
#include <cmath>

Circle::Circle(double cx, double cy, double r) {
    this->cx = cx;
    this->cy = cy;
    this->r = r;
}

double Circle::getArea() {
    return 3.14159 * r * r;
}

Point Circle::getCenter() {
    return Point(cx, cy);
}

void Circle::move(double dx, double dy) {
    cx += dx;
    cy += dy;
}

void Circle::scale(double factor) {
    r = r * factor;
}

std::string Circle::getName() {
    return "CIRCLE";
}
