#include "rectangle.h"
#include <stdexcept>

Rectangle::Rectangle(const Point& bottomLeft, const Point& topRight)
    : bottomLeft_(bottomLeft), topRight_(topRight) {
    if (topRight_.x <= bottomLeft_.x || topRight_.y <= bottomLeft_.y) {
        throw std::invalid_argument("Rectangle: некорректные координаты углов");
    }
}

double Rectangle::getArea() const {
    return (topRight_.x - bottomLeft_.x) * (topRight_.y - bottomLeft_.y);
}

Point Rectangle::getCenter() const {
    return Point{ (bottomLeft_.x + topRight_.x) / 2.0, (bottomLeft_.y + topRight_.y) / 2.0 };
}

void Rectangle::move(double dx, double dy) {
    bottomLeft_.x += dx;
    bottomLeft_.y += dy;
    topRight_.x += dx;
    topRight_.y += dy;
}

void Rectangle::scale(double factor) {
    Point center = getCenter();
    double halfWidth = (topRight_.x - bottomLeft_.x) / 2.0 * factor;
    double halfHeight = (topRight_.y - bottomLeft_.y) / 2.0 * factor;

    bottomLeft_ = Point{ center.x - halfWidth, center.y - halfHeight };
    topRight_ = Point{ center.x + halfWidth, center.y + halfHeight };
}

std::string Rectangle::getName() const {
    return "RECTANGLE";
}
