#ifndef CIRCLE_HPP
#define CIRCLE_HPP

#include "shape.hpp"

class Circle : public Shape {
private:
    Point center_;
    double radius;

public:
    Circle(const Point& c, double r);

    double area() const override;
    Point center() const override;
    void scale(double factor) override;
    void print(std::ostream& os) const override;
};

#endif
