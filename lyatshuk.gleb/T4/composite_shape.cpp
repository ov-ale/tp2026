#include "composite_shape.h"
#include <iostream>
#include <limits>
#include <iomanip>

void CompositeShape::addShape(std::unique_ptr<Shape> shape) {
    shapes.push_back(std::move(shape));
}

double CompositeShape::getArea() const {
    double totalArea = 0;
    for (const auto& shape : shapes) {
        totalArea += shape->getArea();
    }
    return totalArea;
}

Point CompositeShape::getCenter() const {
    return calculateBoundingBoxCenter();
}

Point CompositeShape::calculateBoundingBoxCenter() const {
    if (shapes.empty()) {
        return Point(0, 0);
    }

    double minX = std::numeric_limits<double>::max();
    double minY = std::numeric_limits<double>::max();
    double maxX = std::numeric_limits<double>::lowest();
    double maxY = std::numeric_limits<double>::lowest();

    for (const auto& shape : shapes) {
        Point center = shape->getCenter();
        minX = std::min(minX, center.x);
        minY = std::min(minY, center.y);
        maxX = std::max(maxX, center.x);
        maxY = std::max(maxY, center.y);
    }

    return Point((minX + maxX) / 2, (minY + maxY) / 2);
}

void CompositeShape::move(double dx, double dy) {
    for (auto& shape : shapes) {
        shape->move(dx, dy);
    }
}

void CompositeShape::scale(double factor) {
    Point compositeCenter = getCenter();

    for (auto& shape : shapes) {
        Point shapeCenter = shape->getCenter();

        double dx = shapeCenter.x - compositeCenter.x;
        double dy = shapeCenter.y - compositeCenter.y;

        shape->move(dx * (factor - 1), dy * (factor - 1));
        shape->scale(factor);
    }
}

std::string CompositeShape::getName() const {
    return "COMPOSITE";
}

void CompositeShape::printInfo() const {
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "[" << getName() << ", (" << getCenter().x << ", " << getCenter().y << "), " << getArea() << ":" << std::endl;

    for (const auto& shape : shapes) {
        Point center = shape->getCenter();
        std::cout << "  " << shape->getName() << ", (" << center.x << ", " << center.y << "), " << shape->getArea() << std::endl;
    }

    std::cout << "]" << std::endl;
}
