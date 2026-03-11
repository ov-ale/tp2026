#include "Ring.h"

const double PI = 3.1415926535;

Ring::Ring(Point c, double outerR, double innerR)
{
    center = c;
    outerRadius = outerR;
    innerRadius = innerR;
}

double Ring::getArea() const
{
    return PI * (outerRadius * outerRadius - innerRadius * innerRadius);
}

Point Ring::getCenter() const
{
    return center;
}

std::string Ring::getName() const
{
    return "RING";
}

void Ring::move(double dx, double dy)
{
    center.x += dx;
    center.y += dy;
}

void Ring::scale(double factor)
{
    outerRadius *= factor;
    innerRadius *= factor;
}