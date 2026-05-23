#include "compositeShape.h"
#include <stdexcept>

void CompositeShape::addShape(std::unique_ptr<Shape> shape) {
    shapes_.push_back(std::move(shape));
}

double CompositeShape::getArea() const {
    double total = 0;
    for (const auto& s : shapes_) total += s->getArea();
    return total;
}

Point CompositeShape::getCenter() const {
    if (shapes_.empty()) return Point(0, 0);
    double minX = getMinX(), minY = getMinY(), maxX = getMaxX(), maxY = getMaxY();
    return Point((minX + maxX) / 2.0, (minY + maxY) / 2.0);
}

void CompositeShape::move(double dx, double dy) {
    for (auto& s : shapes_) s->move(dx, dy);
}

void CompositeShape::scale(double factor) {
    if (shapes_.empty()) return;
    Point center = getCenter();
    for (auto& s : shapes_) {
        Point oldC = s->getCenter();
        s->scale(factor);
        s->move((oldC.x - center.x) * (factor - 1), (oldC.y - center.y) * (factor - 1));
    }
}

std::string CompositeShape::getName() const {
    return "COMPOSITE";
}

double CompositeShape::getMinX() const {
    if (shapes_.empty()) return 0;
    double min = shapes_.at(0)->getMinX();
    for (const auto& s : shapes_) if (s->getMinX() < min) min = s->getMinX();
    return min;
}

double CompositeShape::getMinY() const {
    if (shapes_.empty()) return 0;
    double min = shapes_.at(0)->getMinY();
    for (const auto& s : shapes_) if (s->getMinY() < min) min = s->getMinY();
    return min;
}

double CompositeShape::getMaxX() const {
    if (shapes_.empty()) return 0;
    double max = shapes_.at(0)->getMaxX();
    for (const auto& s : shapes_) if (s->getMaxX() > max) max = s->getMaxX();
    return max;
}

double CompositeShape::getMaxY() const {
    if (shapes_.empty()) return 0;
    double max = shapes_.at(0)->getMaxY();
    for (const auto& s : shapes_) if (s->getMaxY() > max) max = s->getMaxY();
    return max;
}
