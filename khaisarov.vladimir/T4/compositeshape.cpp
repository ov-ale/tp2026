#include "compositeshape.h"

CompositeShape::CompositeShape(std::vector<std::unique_ptr<Shape>> s) : shapes(std::move(s)) {}

double CompositeShape::getArea() const
{
    double totalArea = 0;
    for (size_t i = 0; i < shapes.size(); ++i) {
        totalArea += shapes[i]->getArea();
    }
    return totalArea;
}

void CompositeShape::add(std::unique_ptr<Shape> shape)
{
    shapes.push_back(std::move(shape));
}

Point CompositeShape::getCenter() const
{
    if (shapes.empty()) return Point(0, 0);

    double sumX = 0;
    double sumY = 0;
    for (size_t i = 0; i < shapes.size(); ++i) {
        Point p = shapes[i]->getCenter();
        sumX += p.x;
        sumY += p.y;
    }
    return Point(sumX / shapes.size(), sumY / shapes.size());
}

std::string CompositeShape::getName() const
{
    return "COMPOSITE";
}

void CompositeShape::scale(double factor)
{
    Point commonCenter = getCenter();
    for (size_t i = 0; i < shapes.size(); ++i)
    {
        Point shapeCenter = shapes[i]->getCenter();
        double dx = (shapeCenter - commonCenter.x) * (factor - 1);
        double dy = (shapeCenter - commonCenter.y) * (factor - 1);
        shapes[i]->move(dx, dy);
        shapes[i]->scale(factor);
    }
}

void CompositeShape::move(double dx, double dy)
{
    for (size_t i = 0; i < shapes.size(); ++i)
    {
        shapes[i]->move(dx, dy);
    }
}

void CompositeShape::print(std::ostream& os) const
{
    os << std::fixed << std::setprecision(2);
    os << "[" << getName() << ", " << getCenter() << ", " << getArea() << ":\n";
    for (size_t i = 0; i < shapes.size(); ++i)
    {
        os << "  " << shapes[i]->getName() << ", " << shapes[i]->getCenter() << ", " << shapes[i]->getArea() << ",\n";
    }
    os << "]" << std::endl;
}