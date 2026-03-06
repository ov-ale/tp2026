#ifndef RECT_TRAPEZOID_H
#define RECT_TRAPEZOID_H

#include "point.h"
#include "shape.h"
#include <string.h>

class RectangularTrapezoid : public Shape {
private:
    Point bottomLeft;
    double bottomBase;
    double topBase;
    double height;

public:
    RectangularTrapezoid(const Point& bl, double bb, double tb, double h);
    double getArea() const override;
    double getMidlineLength() const;
    Point getCenter() const override;
    void move(double dx, double dy) override;
    void scale(double factor) override;
    std::string getName() const override;
};

#endif // RECT_TRAPEZOID_H
