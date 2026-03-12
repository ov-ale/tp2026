#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "shape.h"

class Rectangle : public Shape {
private:
    double x1, y1;
    double x2, y2;

public:
    Rectangle(double x1, double y1, double x2, double y2);
    double getArea() override;
    Point getCenter() override;
    void move(double dx, double dy) override;
    void scale(double factor) override;
    std::string getName() override;
};

#endif
