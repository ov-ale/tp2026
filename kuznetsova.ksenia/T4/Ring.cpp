#include "Ring.h"
#include <stdexcept>
#include <cmath>
#include <memory>

Ring::Ring(const Point& center, double outerRadius, double innerRadius)
    : center_(center), outerRadius_(outerRadius), innerRadius_(innerRadius)
{
    if (outerRadius <= 0 || innerRadius < 0 || innerRadius >= outerRadius) {
        throw std::invalid_argument("Error: Invalid ring radii.");
    }
}

double Ring::getArea() const
{
    return M_PI * (outerRadius_ * outerRadius_ - innerRadius_ * innerRadius_);
}

Point Ring::getCenter() const
{
    return center_;
}

void Ring::move(double dx, double dy)
{
    center_.x += dx;
    center_.y += dy;
}

void Ring::scale(double factor)
{
    if (factor <= 0) {
        throw std::invalid_argument("Error: Scale factor must be positive.");
    }

    outerRadius_ *= factor;
    innerRadius_ *= factor;
}

std::string Ring::getName() const
{
    return "RING";
}

std::unique_ptr<Shape> Ring::clone() const
{
    return std::make_unique<Ring>(*this);
}
