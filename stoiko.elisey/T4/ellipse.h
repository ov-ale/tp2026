#ifndef ELLIPSE_H
#define ELLIPSE_H

#include "point.h"
#include "shape.h"
#include <string>
#include <cmath>

class Ellipse : public Shape {
private:
    Point center;
    double r1;
    double r2;
public:
    Ellipse(const Point& center, double r1, double r2);
    double getArea() const override;
    Point getCenter() const override;
    void move(double dx, double dy) override;
    void scale(double factor) override;
    std::string getName() const override;
};

#endif // ELLIPSE_H
