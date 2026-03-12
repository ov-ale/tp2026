#ifndef CIRCLE_H
#define CIRCLE_H

#include "shape.h"

class Circle : public Shape {
private:
    double cx, cy; // центр
    double r;      // радиус

public:
    Circle(double cx, double cy, double r);
    double getArea() override;
    Point getCenter() override;
    void move(double dx, double dy) override;
    void scale(double factor) override;
    std::string getName() override;
};

#endif
