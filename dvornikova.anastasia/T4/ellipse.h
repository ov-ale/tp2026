#ifndef ELLIPSE_H
#define ELLIPSE_H

#include "shape.h"

class Ellipse : public Shape {
public:
    Ellipse(const Point& center, double vRadius, double hRadius);
    double getArea() const override;
    Point getCenter() const override;
    void move(double dx, double dy) override;
    void scale(double coefficient) override;
    std::string getName() const override;
    Shape* clone() const override;

private:
    Point center_;
    double vRadius_;
    double hRadius_;
};

#endif