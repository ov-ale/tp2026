#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "shape.h"

// Прямоугольник со сторонами, параллельными осям координат.
class Rectangle : public Shape {
public:
    // bottomLeft - левый нижний угол, topRight - правый верхний угол
    Rectangle(const Point& bottomLeft, const Point& topRight);

    double getArea() const override;
    Point getCenter() const override;
    void move(double dx, double dy) override;
    void scale(double factor) override;
    std::string getName() const override;

private:
    Point bottomLeft_;
    Point topRight_;
};

#endif
