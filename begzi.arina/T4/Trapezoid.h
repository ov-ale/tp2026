#ifndef TRAPEZOID_H
#define TRAPEZOID_H
#include "Shape.h"

class IsoscelesTrapezoid : public Shape {
    Point bl;
    double b_base, t_base, h;
public:
    IsoscelesTrapezoid(Point p, double bb, double tb, double height);
    double getArea() const override;
    Point getCenter() const override;
    void move(double dx, double dy) override;
    void scale(double factor) override;
    std::string getName() const override;
    void getBoundingBox(Point& b, Point& t) const override;
};
#endif