#include "rectangle.h"
#include <stdexcept>

Rectangle::Rectangle(const Point& bottomLeft, const Point& topRight) {
    if (bottomLeft.x >= topRight.x || bottomLeft.y >= topRight.y) {
        throw std::invalid_argument("Invalid coordinates");
    }
    width_ = topRight.x - bottomLeft.x;
    height_ = topRight.y - bottomLeft.y;
    center_.x = bottomLeft.x + width_ / 2.0;
    center_.y = bottomLeft.y + height_ / 2.0;
}

double Rectangle::getArea() const {
    return width_ * height_;
}

Point Rectangle::getCenter() const {
    return center_;
}

void Rectangle::move(double dx, double dy) {
    center_.x += dx;
    center_.y += dy;
}

void Rectangle::scale(double coefficient) {
    if (coefficient <= 0.0) {
        throw std::invalid_argument("Invalid coefficient");
    }
    width_ *= coefficient;
    height_ *= coefficient;
}

std::string Rectangle::getName() const {
    return "RECTANGLE";
}

Shape* Rectangle::clone() const {
    return new Rectangle(*this);
}