#ifndef SHAPE_H
#define SHAPE_H

#include "point.h"
#include <string>

class Shape {
public:
    virtual ~Shape() {}
    virtual double getArea() = 0;
    virtual Point getCenter() = 0;
    virtual void move(double dx, double dy) = 0;
    virtual void scale(double factor) = 0;
    virtual std::string getName() = 0;
};

#endif