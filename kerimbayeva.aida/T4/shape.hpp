#ifndef SHAPE_HPP
#define SHAPE_HPP

#include "point.h"
#include <iostream>

class Shape {
public:
    virtual ~Shape() = default;

    virtual double area() const = 0;
    virtual Point center() const = 0;
    virtual void scale(double factor) = 0;
    virtual void print(std::ostream& os) const = 0;

    friend std::ostream& operator<<(std::ostream& os, const Shape& shape) {
        shape.print(os);
        return os;
    }
};

#endif
