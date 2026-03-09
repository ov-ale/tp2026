#include "Trapezoid.h"
#include <algorithm>

IsoscelesTrapezoid::IsoscelesTrapezoid(Point p, double bb, double tb, double height)
    : bl(p), b_base(bb), t_base(tb), h(height) {
}

double IsoscelesTrapezoid::getArea() const { return (b_base + t_base) / 2.0 * h; }
Point IsoscelesTrapezoid::getCenter() const { return { bl.x + b_base / 2.0, bl.y + h / 2.0 }; }
void IsoscelesTrapezoid::move(double dx, double dy) { bl.x += dx; bl.y += dy; }
void IsoscelesTrapezoid::scale(double f) {
    Point c = getCenter();
    b_base *= f; t_base *= f; h *= f;
    bl.x = c.x - b_base / 2.0; bl.y = c.y - h / 2.0;
}
std::string IsoscelesTrapezoid::getName() const { return "TRAPEZOID"; }
void IsoscelesTrapezoid::getBoundingBox(Point& b, Point& t) const {
    double shift = (b_base - t_base) / 2.0;
    b = { std::min(bl.x, bl.x + shift), bl.y };
    t = { std::max(bl.x + b_base, bl.x + shift + t_base), bl.y + h };
}
