#include "rectangle.h"
#include <stdexcept>
#include <string>

Rectangle::Rectangle(const Point& bottomLeft, const Point& topRight) :
    bottomLeft_(bottomLeft),
    topRight_(topRight)
{
    if (bottomLeft_.x >= topRight_.x || bottomLeft_.y >= topRight_.y) {
        throw std::invalid_argument("Invalid coordinates: bottomLeft must be strictly below and to the left of topRight.");
    }
}

double Rectangle::getArea() const {
    return (topRight_.x - bottomLeft_.x) * (topRight_.y - bottomLeft_.y);
}

Point Rectangle::getCenter() const {
    return Point((bottomLeft_.x + topRight_.x) / 2.0, (bottomLeft_.y + topRight_.y) / 2.0);
}

void Rectangle::move(double dx, double dy) {
    bottomLeft_.x += dx;
    bottomLeft_.y += dy;
    topRight_.x += dx;
    topRight_.y += dy;
}

void Rectangle::scale(double factor) {
    Point center = getCenter();
    bottomLeft_.x = center.x + (bottomLeft_.x - center.x) * factor;
    bottomLeft_.y = center.y + (bottomLeft_.y - center.y) * factor;
    topRight_.x = center.x + (topRight_.x - center.x) * factor;
    topRight_.y = center.y + (topRight_.y - center.y) * factor;
}

std::string Rectangle::getName() const {
    return "RECTANGLE";
}

double Rectangle::getMinX() const { return bottomLeft_.x; }
double Rectangle::getMinY() const { return bottomLeft_.y; }
double Rectangle::getMaxX() const { return topRight_.x; }
double Rectangle::getMaxY() const { return topRight_.y; }
