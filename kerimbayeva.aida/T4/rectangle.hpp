#ifndef RECTANGLE_HPP
#define RECTANGLE_HPP

#include "shape.hpp"

class Rectangle : public Shape {
private:
    Point bottomLeft;
    double width;
    double height;

public:
    Rectangle(const Point& bl, double w, double h);

    double area() const override;
    Point center() const override;
    void scale(double factor) override;
    void print(std::ostream& os) const override;
};

#endif
