#include "ellipse.h"
#include <stdexcept>

namespace {
const double PI = 3.14159265358979323846;
}

Ellipse::Ellipse(const Point& center, double radiusX, double radiusY)
    : center_(center), radiusX_(radiusX), radiusY_(radiusY) {
    if (radiusX_ <= 0.0 || radiusY_ <= 0.0) {
        throw std::invalid_argument("Ellipse: радиусы должны быть положительными");
    }
}

double Ellipse::getArea() const {
    return PI * radiusX_ * radiusY_;
}

Point Ellipse::getCenter() const {
    return center_;
}

void Ellipse::move(double dx, double dy) {
    center_.x += dx;
    center_.y += dy;
}

void Ellipse::scale(double factor) {
    radiusX_ *= factor;
    radiusY_ *= factor;
}

std::string Ellipse::getName() const {
    return "ELLIPSE";
}
