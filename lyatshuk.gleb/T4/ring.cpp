#include "ring.h"
#include <cmath>
#include <stdexcept>

const double PI = 3.14159265358979323846;

Ring::Ring(const Point& center, double outerRadius, double innerRadius)
    : center(center), outerRadius(outerRadius), innerRadius(innerRadius) {
    if (outerRadius <= 0 || innerRadius <= 0 || innerRadius >= outerRadius) {
        throw std::invalid_argument("Invalid radii for Ring");
    }
}

double Ring::getArea() const {
    return PI * (outerRadius * outerRadius - innerRadius * innerRadius);
}

Point Ring::getCenter() const {
    return center;
}

void Ring::move(double dx, double dy) {
    center.x += dx;
    center.y += dy;
}

void Ring::scale(double factor) {
    outerRadius *= factor;
    innerRadius *= factor;
}

std::string Ring::getName() const {
    return "RING";
}
