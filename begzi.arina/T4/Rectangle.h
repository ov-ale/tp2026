#ifndef RECTANGLE_H
#define RECTANGLE_H
#include "Shape.h"

class Rectangle : public Shape {
    Point bl, tr;
public:
    Rectangle(Point bottom_left, Point top_right);
    double getArea() const override;
    Point getCenter() const override;
    void move(double dx, double dy) override;
    void scale(double factor) override;
    std::string getName() const override;
    void getBoundingBox(Point& b, Point& t) const override;
};
#endif