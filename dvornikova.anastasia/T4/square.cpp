#include "square.h"
#include <stdexcept>

Square::Square(const Point& bottomLeft, double side) : side_(side) {
    if (side_ <= 0.0) {
        throw std::invalid_argument("Invalid side");
    }
    center_.x = bottomLeft.x + side_ / 2.0;
    center_.y = bottomLeft.y + side_ / 2.0;
}

double Square::getArea() const {
    return side_ * side_;
}

Point Square::getCenter() const {
    return center_;
}

void Square::move(double dx, double dy) {
    center_.x += dx;
    center_.y += dy;
}

void Square::scale(double coefficient) {
    if (coefficient <= 0.0) {
        throw std::invalid_argument("Invalid coefficient");
    }
    side_ *= coefficient;
}

std::string Square::getName() const {
    return "SQUARE";
}

Shape* Square::clone() const {
    return new Square(*this);
}