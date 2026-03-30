#include "circle.hpp"
#include <iomanip>
#include <stdexcept>

const double pi = 3.14159265358979323846;

Circle::Circle(const Point& c, double r)
    : center_(c), radius(r) {
    if (r <= 0) {
        throw std::invalid_argument("Radius must be positive");
    }
}

double Circle::area() const {
    return pi * radius * radius;
}

Point Circle::center() const {
    return center_;
}

void Circle::scale(double factor) {
    if (factor <= 0) {
        throw std::invalid_argument("Scale factor must be positive");
    }
    radius *= factor;
}

void Circle::print(std::ostream& os) const {
    os << std::fixed << std::setprecision(2);
    os << "CIRCLE, (" << center_.x << ", " << center_.y << "), " << radius;
}
