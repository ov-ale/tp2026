#ifndef CIRCLE_H
#define CIRCLE_H

#include "Shape.h"
#include <stdexcept>

class Circle : public Shape {
private:
    Point center_;
    double radius_;

public:
    Circle(const Point& center, double radius) : center_(center), radius_(radius) {
        if (radius_ <= 0) {
            throw std::invalid_argument("Circle: radius must be positive");
        }
    }

    double getArea() const override {
        const double PI = 3.14159265358979323846;
        return PI * radius_ * radius_;
    }
    Point getCenter() const override {
        return center_;
    }
    void move(double dx, double dy) override {
        center_.x_ += dx;
        center_.y_ += dy;
    }
    void scale(double k) override {
        if (k <= 0) {
            throw std::invalid_argument("Circle::scale: coefficient must be positive");
        }
        radius_ *= k;
    }
    std::string getName() const override {
        return "CIRCLE";
    }
    void print(std::ostream& os) const override {
        os << "[CIRCLE, " << center_ << ", " << getArea() << "]";
    }
    Bounds getBounds() const override {
        return Bounds(center_.x_ - radius_,
            center_.y_ - radius_,
            center_.x_ + radius_,
            center_.y_ + radius_);
    }
};

#endif
