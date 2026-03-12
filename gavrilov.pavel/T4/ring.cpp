#include "ring.h"
#include <cmath>
#include <stdexcept>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Ring::Ring(const Point& center, double outerRadius, double innerRadius)
    : center(center), outerRadius(outerRadius), innerRadius(innerRadius) {
    if (outerRadius <= innerRadius) {
        this->outerRadius = innerRadius + 1.0;
    }
}

double Ring::getArea() const {
    return M_PI * (outerRadius * outerRadius - innerRadius * innerRadius);
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
