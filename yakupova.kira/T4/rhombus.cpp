#include "rhombus.h"

Rhombus::Rhombus(const Point& center, double diagX, double diagY)
    : center(center), diagX(diagX), diagY(diagY) {}

double Rhombus::getArea() const {
    return (diagX * diagY) / 2.0;
}

Point Rhombus::getCenter() const {
    return center;
}

void Rhombus::move(double dx, double dy) {
    center.x += dx;
    center.y += dy;
}

void Rhombus::scale(double factor) {
    diagX *= factor;
    diagY *= factor;
}

std::string Rhombus::getName() const {
    return "RHOMBUS";
}