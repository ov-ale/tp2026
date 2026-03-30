#ifndef RIGHT_TRAPEZOID_HPP
#define RIGHT_TRAPEZOID_HPP

#include "shape.hpp"

class RightTrapezoid : public Shape {
private:
    Point bottomLeft;
    double bottomBase;
    double topBase;
    double height;

public:
    RightTrapezoid(const Point& bl, double bBase, double tBase, double h);

    double area() const override;
    Point center() const override;
    void scale(double factor) override;
    void print(std::ostream& os) const override;
};

#endif
