#include "rectangularTrapezoid.h"
#include <stdexcept>
#include <algorithm>

rectangularTrapezoid::rectangularTrapezoid(const Point& bl, double bottomW, double topW, double h)
    : bottomLeft(bl), bottomWidth(bottomW), topWidth(topW), height(h)
{
    if (bottomWidth <= 0.0 || topWidth <= 0.0 || height <= 0.0)
    {
        throw std::invalid_argument("Invalid values");
    }
}

double rectangularTrapezoid::getArea() const
{
    return (bottomWidth + topWidth) / 2.0 * height;
}

Point rectangularTrapezoid::getCenter() const
{
    double centerX = bottomLeft.x + (bottomWidth + topWidth) / 4.0;
    double centerY = bottomLeft.y + height / 2.0;
    return Point(centerX, centerY);
}

void rectangularTrapezoid::move(double dx, double dy)
{
    bottomLeft.x += dx;
    bottomLeft.y += dy;
}

void rectangularTrapezoid::scale(double factor)
{
    if (factor <= 0.0)
    {
        throw std::invalid_argument("Scale factor must be positive");
    }

    Point center = getCenter();

    bottomWidth *= factor;
    topWidth *= factor;
    height *= factor;

    bottomLeft.x = center.x + (bottomLeft.x - center.x) * factor;
    bottomLeft.y = center.y + (bottomLeft.y - center.y) * factor;
}

std::string rectangularTrapezoid::getName() const
{
    return "RECTANGULAR_TRAPEZOID";
}

std::pair<Point, Point> rectangularTrapezoid::getFrame() const
{
    double minX = bottomLeft.x;
    double maxX = bottomLeft.x + std::max(bottomWidth, topWidth);
    double minY = bottomLeft.y;
    double maxY = bottomLeft.y + height;

    return { Point(minX, minY), Point(maxX, maxY) };
}