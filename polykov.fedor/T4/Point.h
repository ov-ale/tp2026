#ifndef POINT_H
#define POINT_H

#include <iostream>

struct Point {
    double x_;
    double y_;

    Point() : x_(0.0), y_(0.0) {
    }
    Point(double x, double y) : x_(x), y_(y) {
    }

    Point operator+(const Point& other) const {
        return Point(x_ + other.x_, y_ + other.y_);
    }
    Point operator-(const Point& other) const {
        return Point(x_ - other.x_, y_ - other.y_);
    }
    Point operator*(double scalar) const {
        return Point(x_ * scalar, y_ * scalar);
    }
    void print(std::ostream& os) const {
        os << "(" << x_ << ", " << y_ << ")";
    }
};

std::ostream& operator<<(std::ostream& os, const Point& p) {
    p.print(os);
    return os;
}

#endif
