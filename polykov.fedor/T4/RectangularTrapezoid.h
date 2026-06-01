#ifndef RECTANGULAR_TRAPEZOID_H
#define RECTANGULAR_TRAPEZOID_H

#include <algorithm>
#include "Shape.h"
#include <stdexcept>

class RectangularTrapezoid : public Shape {
private:
    Point leftBottom_;
    double bottomBase_;
    double topBase_;
    double height_;

public:
    RectangularTrapezoid(const Point& leftBottom, double bottomBase, double topBase, double height)
        : leftBottom_(leftBottom), bottomBase_(bottomBase), topBase_(topBase), height_(height)
    {
        if (bottomBase_ <= 0 || topBase_ <= 0 || height_ <= 0) {
            throw std::invalid_argument("RectangularTrapezoid: bases and height must be positive");
        }
    }

    double getArea() const override {
        return (bottomBase_ + topBase_) / 2.0 * height_;
    }
    Point getCenter() const override {
        double centerX = leftBottom_.x_ + (bottomBase_ + topBase_) / 4.0;
        double centerY = leftBottom_.y_ + height_ / 2.0;
        return Point(centerX, centerY);
    }
    void move(double dx, double dy) override {
        leftBottom_.x_ += dx;
        leftBottom_.y_ += dy;
    }
    void scale(double k) override {
        if (k <= 0) {
            throw std::invalid_argument("RectangularTrapezoid::scale: coefficient must be positive");
        }
        Point center = getCenter();
        bottomBase_ *= k;
        topBase_ *= k;
        height_ *= k;
        leftBottom_ = center + (leftBottom_ - center) * k;
    }
    std::string getName() const override {
        return "RECTANGULAR_TRAPEZOID";
    }
    void print(std::ostream& os) const override {
        Point center = getCenter();
        os << "[RECTANGULAR_TRAPEZOID, " << center << ", " << getArea() << "]";
    }
    Bounds getBounds() const override {
        double maxX = leftBottom_.x_ + std::max(bottomBase_, topBase_);
        double maxY = leftBottom_.y_ + height_;
        return Bounds(leftBottom_.x_, leftBottom_.y_, maxX, maxY);
    }
};

#endif
