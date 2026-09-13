#ifndef RHOMBUS_H
#define RHOMBUS_H

#include "shape.h"

class Rhombus : public Shape {
public:
    Rhombus(const Point& center, double diagonalX, double diagonalY);

    double getArea() const override;
    Point getCenter() const override;
    void move(double dx, double dy) override;
    void scale(double factor) override;
    std::string getName() const override;

private:
    Point center_;
    double diagonalX_;
    double diagonalY_;
};

#endif
