#include "ellipse.h"
#include <cmath>

Ellipse::Ellipse(const Point& center, double radiusX, double radiusY)
    : center(center), radiusX(radiusX), radiusY(radiusY) {}

double Ellipse::getArea() const {
    return M_PI * radiusX * radiusY;
}

Point Ellipse::getCenter() const {
    return center;
}

void Ellipse::move(double dx, double dy) {
    center.x += dx;
    center.y += dy;
}

void Ellipse::scale(double factor) {
    radiusX *= factor;
    radiusY *= factor;
}

std::string Ellipse::getName() const {
    return "ELLIPSE";
}
