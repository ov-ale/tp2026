#include "Composite_Shape.h"
#include <algorithm>
#include <iomanip>
#include <iostream>

void CompositeShape::addShape(std::unique_ptr<Shape> shape) {
    shapes_.push_back(std::move(shape));
}

double CompositeShape::getArea() const {
    double totalArea = 0;
    for (size_t i = 0; i < shapes_.size(); i++) {
        totalArea += shapes_[i]->getArea();
    }
    return totalArea;
}

Point CompositeShape::getCenter() const {
    if (shapes_.empty()) {
        return Point(0, 0);
    }

    double minX = shapes_[0]->getCenter().x_;
    double maxX = minX;
    double minY = shapes_[0]->getCenter().y_;
    double maxY = minY;

    for (size_t i = 0; i < shapes_.size(); i++) {
        Point center = shapes_[i]->getCenter();

        minX = std::min(minX, center.x_);
        maxX = std::max(maxX, center.x_);

        minY = std::min(minY, center.y_);
        maxY = std::max(maxY, center.y_);
    }
    return Point((minX + maxX) / 2, (minY + maxY) / 2);
}

void CompositeShape::move(double dx, double dy) {
    for (size_t i = 0; i < shapes_.size(); i++) {
        shapes_[i]->move(dx, dy);
    }
}

void CompositeShape::scale(double factor) {
    Point center = getCenter();

    for (size_t i = 0; i < shapes_.size(); i++) {
        Point shapeCenter = shapes_[i]->getCenter();
        double newCenterX = center.x_ + (shapeCenter.x_ - center.x_) * factor;
        double newCenterY = center.y_ + (shapeCenter.y_ - center.y_) * factor;

        double dx = newCenterX - shapeCenter.x_;
        double dy = newCenterY - shapeCenter.y_;

        shapes_[i]->move(dx, dy);
        shapes_[i]->scale(factor);
    }
}

std::string CompositeShape::getName() const {
    return "COMPOSITE";
}

void CompositeShape::print() const {
    Point center = getCenter();
    std::cout << std::fixed << std::setprecision(2);

    std::cout << "[" << getName()
            << ", (" << center.x_ << ", " << center.y_ << "), "
            << getArea() << ":\n";

    for (size_t i = 0; i < shapes_.size(); i++) {
        Point shapeCenter = shapes_[i]->getCenter();
        std::cout << "  "
                << shapes_[i]->getName()
                << ", (" << shapeCenter.x_ << ", " << shapeCenter.y_ << "), "
                << shapes_[i]->getArea() << ",\n";
    }
    std::cout << "]\n";
}
