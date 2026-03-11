#include "IsoscelesTrapeziod.h"

IsoscelesTrapeziod::IsoscelesTrapeziod(Point bl, double bBase, double tBase, double h)
{
    bottomLeft = bl;
    bottomBase = bBase;
    topBase = tBase;
    height = h;
}

double IsoscelesTrapeziod::getArea() const
{
    return (bottomBase + topBase) * height / 2.0;
}

Point IsoscelesTrapeziod::getCenter() const
{
    double cx = bottomLeft.x + bottomBase / 2.0;
    double cy = bottomLeft.y + height / 2.0;
    return Point(cx, cy);
}

std::string IsoscelesTrapeziod::getName() const
{
    return "ISOSCELESTRAPEZOID";
}

void IsoscelesTrapeziod::move(double dx, double dy)
{
    bottomLeft.x += dx;
    bottomLeft.y += dy;
}

void IsoscelesTrapeziod::scale(double factor)
{
    bottomBase *= factor;
    topBase *= factor;
    height *= factor;
}

