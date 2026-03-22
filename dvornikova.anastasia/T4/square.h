#ifndef SQUARE_H
#define SQUARE_H

#include "shape.h"

class Square : public Shape {
public:
    Square(const Point& bottomLeft, double side);
    double getArea() const override;
    Point getCenter() const override;
    void move(double dx, double dy) override;
    void scale(double coefficient) override;
    std::string getName() const override;
    Shape* clone() const override;

private:
    Point center_;
    double side_;
};

#endif