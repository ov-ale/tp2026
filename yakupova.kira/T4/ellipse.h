#ifndef ELLIPSE_H
#define ELLIPSE_H

#include "shape.h"

// Эллипс с осями, параллельными осям координат.
class Ellipse : public Shape {
public:
    // center - центр эллипса
    // radiusX - радиус по горизонтальной оси, radiusY - радиус по вертикальной оси
    Ellipse(const Point& center, double radiusX, double radiusY);

    double getArea() const override;
    Point getCenter() const override;
    void move(double dx, double dy) override;
    void scale(double factor) override;
    std::string getName() const override;

private:
    Point center_;
    double radiusX_;
    double radiusY_;
};

#endif
