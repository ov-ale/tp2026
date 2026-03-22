#include "ellipse.h"
#include <cmath>
#include <stdexcept>

Ellipse::Ellipse(const Point& center, double vRadius, double hRadius) :
    center_(center), vRadius_(vRadius), hRadius_(hRadius) {
    if (vRadius_ <= 0.0 || hRadius_ <= 0.0) {
        throw std::invalid_argument("Invalid radii");
    }
}

double Ellipse::getArea() const {
    return std::acos(-1.0) * vRadius_ * hRadius_;
}

Point Ellipse::getCenter() const {
    return center_;
}

void Ellipse::move(double dx, double dy) {
    center_.x += dx;
    center_.y += dy;
}

void Ellipse::scale(double coefficient) {
    if (coefficient <= 0.0) {
        throw std::invalid_argument("Invalid coefficient");
    }
    vRadius_ *= coefficient;
    hRadius_ *= coefficient;
}

std::string Ellipse::getName() const {
    return "ELLIPSE";
}

Shape* Ellipse::clone() const {
    return new Ellipse(*this);
}