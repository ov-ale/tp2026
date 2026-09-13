#include "compositeshape.h"
#include <algorithm>
#include <stdexcept>

void CompositeShape::addShape(std::unique_ptr<Shape> shape) {
    if (!shape) {
        throw std::invalid_argument("CompositeShape::addShape: пустой указатель");
    }
    shapes_.push_back(std::move(shape));
}

double CompositeShape::getArea() const {
    double total = 0.0;
    for (const auto& shape : shapes_) {
        total += shape->getArea();
    }
    return total;
}

Point CompositeShape::getCenter() const {
    if (shapes_.empty()) {
        return Point{0.0, 0.0};
    }

    Point first = shapes_.front()->getCenter();
    double minX = first.x;
    double maxX = first.x;
    double minY = first.y;
    double maxY = first.y;

    for (const auto& shape : shapes_) {
        Point c = shape->getCenter();
        minX = std::min(minX, c.x);
        maxX = std::max(maxX, c.x);
        minY = std::min(minY, c.y);
        maxY = std::max(maxY, c.y);
    }

    return Point{ (minX + maxX) / 2.0, (minY + maxY) / 2.0 };
}

void CompositeShape::move(double dx, double dy) {
    for (auto& shape : shapes_) {
        shape->move(dx, dy);
    }
}

void CompositeShape::scale(double factor) {
    Point center = getCenter();

    for (auto& shape : shapes_) {
        Point shapeCenter = shape->getCenter();
        double dx = (shapeCenter.x - center.x) * (factor - 1.0);
        double dy = (shapeCenter.y - center.y) * (factor - 1.0);
        shape->move(dx, dy);
        shape->scale(factor);
    }
}

std::string CompositeShape::getName() const {
    return "COMPOSITE";
}

size_t CompositeShape::getCount() const {
    return shapes_.size();
}

const Shape& CompositeShape::getShapeAt(size_t index) const {
    if (index >= shapes_.size()) {
        throw std::out_of_range("CompositeShape::getShapeAt: индекс вне диапазона");
    }
    return *shapes_[index];
}
