#include "Ring.h"
#include <stdexcept>

Ring::Ring(const Point& c, double outerR, double innerR)
    : center(c), outerRadius(outerR), innerRadius(innerR) {
    if (outerR <= innerR) {
        throw std::invalid_argument("Outer radius must be greater than inner radius");
    }
    if (outerR <= 0 || innerR < 0) {
        throw std::invalid_argument("Radii must be positive");
    }
}

double Ring::getArea() const {
    return 3.14159265358979323846 * (outerRadius * outerRadius - innerRadius * innerRadius);
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

double Ring::getOuterRadius() const {
    return outerRadius;
}

double Ring::getInnerRadius() const {
    return innerRadius;
}
