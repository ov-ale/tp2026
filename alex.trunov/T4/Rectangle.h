#include <iostream>
#include <string>
#include "Shape.h"

#ifndef RECTANGLE_H
#define RECTANGLE_H

class Rectangle : public Shape {
public:
    Rectangle() = default;
    Rectangle(Point a, Point b) :
        bottomLeft_(a), topRight_(b)
    { }

    double getArea() const override;
    Point getCenter() const override;
    void move(double x, double y) override;
    void scale(double factor) override;
    std::string getName() const override;

    // help func
    Point getTopRight() const override { return topRight_; }
    Point getBottomLeft() const override { return bottomLeft_; }
private:
    Point topRight_, bottomLeft_;
};

#endif // !RECTANGLE_H
