#include "square.h"
#include <string>

Square::Square(const Point& bottomLeft, double side) :
    bottomLeft_(bottomLeft), side_(side)
{ }

double Square::getArea() const {
    return side_ * side_;
}

Point Square::getCenter() const {
    return Point(bottomLeft_.x + side_ / 2.0, bottomLeft_.y + side_ / 2.0);
}

void Square::move(double dx, double dy) {
    bottomLeft_.x += dx;
    bottomLeft_.y += dy;
}

void Square::scale(double factor) {
    Point center = getCenter();
    side_ *= factor;
    bottomLeft_.x = center.x - side_ / 2.0;
    bottomLeft_.y = center.y - side_ / 2.0;
}

std::string Square::getName() const {
    return "SQUARE";
}

double Square::getMinX() const { return bottomLeft_.x; }
double Square::getMinY() const { return bottomLeft_.y; }
double Square::getMaxX() const { return bottomLeft_.x + side_; }
double Square::getMaxY() const { return bottomLeft_.y + side_; }
