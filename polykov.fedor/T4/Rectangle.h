#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "Shape.h"
#include <stdexcept>

class Rectangle : public Shape {
private:
    Point lowerLeft_;
    Point upperRight_;

public:
    Rectangle(const Point& lowerLeft, const Point& upperRight)
        : lowerLeft_(lowerLeft), upperRight_(upperRight)
    {
        if (lowerLeft_.x_ > upperRight_.x_ || lowerLeft_.y_ > upperRight_.y_) {
            throw std::invalid_argument("Rectangle: lowerLeft must be <= upperRight");
        }
    }

    double getArea() const override {
        return (upperRight_.x_ - lowerLeft_.x_) * (upperRight_.y_ - lowerLeft_.y_);
    }
    Point getCenter() const override {
        return Point((lowerLeft_.x_ + upperRight_.x_) / 2.0,
            (lowerLeft_.y_ + upperRight_.y_) / 2.0);
    }
    void move(double dx, double dy) override {
        lowerLeft_.x_ += dx;
        lowerLeft_.y_ += dy;
        upperRight_.x_ += dx;
        upperRight_.y_ += dy;
    }
    void scale(double k) override {
        if (k <= 0) {
            throw std::invalid_argument("Rectangle::scale: coefficient must be positive");
        }
        Point center = getCenter();
        lowerLeft_ = center + (lowerLeft_ - center) * k;
        upperRight_ = center + (upperRight_ - center) * k;
    }
    std::string getName() const override {
        return "RECTANGLE";
    }
    void print(std::ostream& os) const override {
        Point center = getCenter();
        os << "[RECTANGLE, " << center << ", " << getArea() << "]";
    }
    Bounds getBounds() const override {
        return Bounds(lowerLeft_.x_, lowerLeft_.y_, upperRight_.x_, upperRight_.y_);
    }
};

#endif
