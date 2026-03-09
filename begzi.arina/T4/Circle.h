#ifndef CIRCLE_H
#define CIRCLE_H
#include "Shape.h"

class Circle : public Shape {
    Point pos;
    double r;
public:
    Circle(Point center, double radius);
    double getArea() const override;
    Point getCenter() const override;
    void move(double dx, double dy) override;
    void scale(double factor) override;
    std::string getName() const override;
    void getBoundingBox(Point& bl, Point& tr) const override;
};
#endif