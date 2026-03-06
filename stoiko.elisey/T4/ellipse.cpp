#include "ellipse.h"

Ellipse::Ellipse(const Point& center, double r1, double r2)
    : center(center)
    , r1(r1)
    , r2(r2)
{}

double Ellipse::getArea() const {
    return M_PI * r1 * r2;
}

Point Ellipse::getCenter() const {
    return center;
}

void Ellipse::move(double dx, double dy)  {
    center.x += dx;
    center.y += dy;
}

void Ellipse::scale(double factor)  {
    r1 *= factor;
    r2 *= factor;
}

std::string Ellipse::getName() const {
    return "ELLIPSE";
}
