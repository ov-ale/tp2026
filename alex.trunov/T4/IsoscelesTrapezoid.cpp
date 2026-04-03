#include "IsoscelesTrapezoid.h"
#include <cmath>
#include <string>

double IsoscelesTrapezoid::getArea() const {
    double sq = height_ * (bottomBase_ + topBase_) / 2;
    return sq;
}

Point IsoscelesTrapezoid::getCenter() const {
    double centerX = bottomLeft_.x_ + (bottomBase_ / 2.0) + (topBase_ - bottomBase_) / 4.0;
    double centerY = bottomLeft_.y_ + height_ / 2.0;
    return Point(centerX, centerY);
}

void IsoscelesTrapezoid::move(double x, double y) {
    bottomLeft_.x_ += x;
    bottomLeft_.y_ += y;
}

void IsoscelesTrapezoid::scale(double factor) {
    Point center = getCenter();

    bottomBase_ *= factor;
    topBase_ *= factor;
    height_ *= factor;

    Point newCenter = getCenter();
    double dx = center.x_ - newCenter.x_;
    double dy = center.y_ - newCenter.y_;
    move(dx, dy);
}

std::string IsoscelesTrapezoid::getName() const {
    return "Isosceles trapezoid";
}
