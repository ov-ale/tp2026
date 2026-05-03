#include "composite_shape.h"
#include <algorithm>
#include <limits>
#include <stdexcept>

void CompositeShape::addShape(std::unique_ptr<Shape> shape) {
    if (shape) {
        shapes_.push_back(std::move(shape));
    }
}
double CompositeShape::getArea() const {
    double total = 0.0;
    for (const auto& s : shapes_) {
        total += s->getArea();
    }
    return total;
}
Point CompositeShape::getCenter() const {
    if (shapes_.empty()) {
        return Point(0, 0);
    }
    double minX = std::numeric_limits<double>::max();
    double minY = std::numeric_limits<double>::max();
    double maxX = -std::numeric_limits<double>::max();
    double maxY = -std::numeric_limits<double>::max();
    for (const auto& s : shapes_) {
        Point c = s->getCenter();
        if (c.x < minX) minX = c.x;
        if (c.y < minY) minY = c.y;
        if (c.x > maxX) maxX = c.x;
        if (c.y > maxY) maxY = c.y;
    }
    return Point((minX + maxX) / 2.0, (minY + maxY) / 2.0);
}
void CompositeShape::move(double dx, double dy) {
    for (auto& s : shapes_) {
        s->move(dx, dy);
    }
}
void CompositeShape::scale(double factor) {
    if (factor <= 0.0) {
        throw std::invalid_argument("scale factor must be positive");
    }
    if (shapes_.empty()) {
        return;
    }
    Point center = getCenter();
    for (auto& s : shapes_) {
        Point c = s->getCenter();
        double dx = c.x - center.x;
        double dy = c.y - center.y;
        s->move(dx * (factor - 1.0), dy * (factor - 1.0));
        s->scale(factor);
    }
}
std::string CompositeShape::getName() const {
    return "COMPOSITE";
}
bool CompositeShape::containsPoint(const Point& p) const {
    for (const auto& s : shapes_) {
        if (s->containsPoint(p)) {
            return true;
        }
    }
    return false;
}
double CompositeShape::getPerimeter() const {
    double total = 0.0;
    for (const auto& s : shapes_) {
        total += s->getPerimeter();
    }
    return total;
}
void CompositeShape::getBoundingBox(Point& min, Point& max) const {
    if (shapes_.empty()) {
        min = Point(0, 0);
        max = Point(0, 0);
        return;
    }
    Point shapeMin, shapeMax;
    shapes_[0]->getBoundingBox(min, max);
    for (size_t i = 1; i < shapes_.size(); ++i) {
        shapes_[i]->getBoundingBox(shapeMin, shapeMax);
        min.x = std::min(min.x, shapeMin.x);
        min.y = std::min(min.y, shapeMin.y);
        max.x = std::max(max.x, shapeMax.x);
        max.y = std::max(max.y, shapeMax.y);
    }
}
//

