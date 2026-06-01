#ifndef SHAPE_H
#define SHAPE_H

#include <iostream>
#include <string>
#include "Point.h"

struct Bounds {
    double left, bottom, right, top;
    Bounds(double l = 0, double b = 0, double r = 0, double t = 0)
        : left(l), bottom(b), right(r), top(t) {
    }
    Point center() const {
        return Point((left + right) / 2.0, (bottom + top) / 2.0);
    }
};

class Shape {
public:
    virtual ~Shape() = default;
    virtual double getArea() const = 0;
    virtual Point getCenter() const = 0;
    virtual void move(double dx, double dy) = 0;
    virtual void scale(double k) = 0;
    virtual std::string getName() const = 0;
    virtual void print(std::ostream& os) const = 0;
    virtual Bounds getBounds() const = 0;
};

#endif
