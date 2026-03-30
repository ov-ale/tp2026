#include "right_trapezoid.hpp"
#include <iomanip>
#include <stdexcept>

RightTrapezoid::RightTrapezoid(const Point& bl, double bBase, double tBase, double h)
    : bottomLeft(bl), bottomBase(bBase), topBase(tBase), height(h) {
    if (bBase <= 0 || tBase <= 0 || h <= 0) {
        throw std::invalid_argument("Bases and height must be positive");
    }
}

double RightTrapezoid::area() const {
    return (bottomBase + topBase) / 2.0 * height;
}

Point RightTrapezoid::center() const {
    double midX = bottomLeft.x + (bottomBase + topBase) / 4.0;
    double midY = bottomLeft.y + height / 2.0;
    return Point(midX, midY);
}

void RightTrapezoid::scale(double factor) {
    if (factor <= 0) {
        throw std::invalid_argument("Scale factor must be positive");
    }

    Point c = center();

    bottomBase *= factor;
    topBase *= factor;
    height *= factor;

    bottomLeft.x = c.x - (bottomBase + topBase) / 4.0;
    bottomLeft.y = c.y - height / 2.0;
}

void RightTrapezoid::print(std::ostream& os) const {
    os << std::fixed << std::setprecision(2);
    os << "RIGHT_TRAPEZOID, (" << bottomLeft.x << ", " << bottomLeft.y << "), "
        << bottomBase << ", " << topBase << ", " << height;
}
