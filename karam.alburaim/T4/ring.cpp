#include "ring.h"
#include <stdexcept>
#include <cmath>
#include <string>

const double PI = 3.141592653589793;

Ring::Ring(const Point& center, double outer, double inner) :
    center_(center), outerRadius_(outer), innerRadius_(inner)
{
    if (outerRadius_ <= 0 || innerRadius_ < 0 || innerRadius_ >= outerRadius_) {
        throw std::invalid_argument("Invalid ring radii: outer must be strictly greater than inner, and both must be positive.");
    }
}

double Ring::getArea() const {
    return PI * (outerRadius_ * outerRadius_ - innerRadius_ * innerRadius_);
}

Point Ring::getCenter() const {
    return center_;
}

void Ring::move(double dx, double dy) {
    center_.x += dx;
    center_.y += dy;
}

void Ring::scale(double factor) {
    outerRadius_ *= factor;
    innerRadius_ *= factor;
}

std::string Ring::getName() const {
    return "RING";
}

double Ring::getMinX() const { return center_.x - outerRadius_; }
double Ring::getMinY() const { return center_.y - outerRadius_; }
double Ring::getMaxX() const { return center_.x + outerRadius_; }
double Ring::getMaxY() const { return center_.y + outerRadius_; }
