#include "CompositeShape.h"

void CompositeShape::addShape(std::unique_ptr<Shape> shape)
{
    shapes.push_back(std::move(shape));
}

double CompositeShape::getArea() const
{
    double sum = 0;

    for (const auto& s : shapes)
        sum += s->getArea();

    return sum;
}

Point CompositeShape::getCenter() const
{
    double x = 0;
    double y = 0;

    for (const auto& s : shapes)
    {
        Point c = s->getCenter();
        x += c.x;
        y += c.y;
    }

    x /= shapes.size();
    y /= shapes.size();

    return Point(x, y);
}

std::string CompositeShape::getName() const
{
    return "CompositeShape";
}

void CompositeShape::move(double dx, double dy)
{
    for (auto& s : shapes)
        s->move(dx, dy);
}

void CompositeShape::scale(double factor)
{
    Point center = getCenter();

    for (auto& s : shapes)
    {
        Point sc = s->getCenter();

        double dx = sc.x - center.x;
        double dy = sc.y - center.y;

        double newDx = dx * factor;
        double newDy = dy * factor;

        double newX = center.x + newDx;
        double newY = center.y + newDy;

        s->move(newX - sc.x, newY - sc.y);

        s->scale(factor);
    }
}