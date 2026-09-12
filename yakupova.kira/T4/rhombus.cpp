#include "rhombus.h"
#include <stdexcept>

Rhombus::Rhombus(const Point& center, double diagonalX, double diagonalY)
    : center_(center), diagonalX_(diagonalX), diagonalY_(diagonalY) {
    if (diagonalX_ <= 0.0 || diagonalY_ <= 0.0) {
        throw std::invalid_argument("Rhombus: диагонали должны быть положительными");
    }
}

double Rhombus::getArea() const {
    return (diagonalX_ * diagonalY_) / 2.0;
}

Point Rhombus::getCenter() const {
    return center_;
}

void Rhombus::move(double dx, double dy) {
    center_.x += dx;
    center_.y += dy;
}

void Rhombus::scale(double factor) {
    diagonalX_ *= factor;
    diagonalY_ *= factor;
}

std::string Rhombus::getName() const {
    return "RHOMBUS";
}
