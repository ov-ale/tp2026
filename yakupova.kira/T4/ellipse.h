#ifndef ELLIPSE_H
#define ELLIPSE_H

#include "shape.h"

class Ellipse : public Shape {
private:
    Point center;
    double radiusX;  // радиус по горизонтальной оси
    double radiusY;  // радиус по вертикальной оси
    
public:
    Ellipse(const Point& center, double radiusX, double radiusY);
    
    double getArea() const override;
    Point getCenter() const override;
    void move(double dx, double dy) override;
    void scale(double factor) override;
    std::string getName() const override;
};

#endif 