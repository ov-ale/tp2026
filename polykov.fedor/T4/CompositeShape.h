#ifndef COMPOSITE_SHAPE_H
#define COMPOSITE_SHAPE_H

#include "Shape.h"
#include <vector>
#include <memory>
#include <algorithm>
#include <stdexcept>

class CompositeShape : public Shape {
private:
    std::vector<std::unique_ptr<Shape>> shapes_;

public:
    CompositeShape() = default;
    CompositeShape(const CompositeShape&) = delete;
    CompositeShape& operator=(const CompositeShape&) = delete;
    CompositeShape(CompositeShape&&) = default;
    CompositeShape& operator=(CompositeShape&&) = default;

    void addShape(std::unique_ptr<Shape> shape) {
        shapes_.push_back(std::move(shape));
    }
    double getArea() const override {
        double total = 0.0;
        for (const std::unique_ptr<Shape>& shape : shapes_) {
            total += shape->getArea();
        }
        return total;
    }
    Point getCenter() const override {
        if (shapes_.empty()) {
            throw std::logic_error("CompositeShape::getCenter: no shapes");
        }
        Bounds totalBounds = shapes_[0]->getBounds();
        for (size_t i = 1; i < shapes_.size(); ++i) {
            Bounds b = shapes_[i]->getBounds();
            totalBounds.left = std::min(totalBounds.left, b.left);
            totalBounds.bottom = std::min(totalBounds.bottom, b.bottom);
            totalBounds.right = std::max(totalBounds.right, b.right);
            totalBounds.top = std::max(totalBounds.top, b.top);
        }
        return totalBounds.center();
    }
    void move(double dx, double dy) override {
        for (std::unique_ptr<Shape>& shape : shapes_) {
            shape->move(dx, dy);
        }
    }
    void scale(double k) override {
        if (k <= 0) {
            throw std::invalid_argument("CompositeShape::scale: coefficient must be positive");
        }
        Point commonCenter = getCenter();
        for (std::unique_ptr<Shape>& shape : shapes_) {
            Point oldCenter = shape->getCenter();
            Point newCenter = commonCenter + (oldCenter - commonCenter) * k;
            shape->move(newCenter.x_ - oldCenter.x_, newCenter.y_ - oldCenter.y_);
            shape->scale(k);
        }
    }
    std::string getName() const override {
        return "COMPOSITE";
    }
    void print(std::ostream& os) const override {
        Point center = getCenter();
        os << "[COMPOSITE, " << center << ", " << getArea() << ":\n";
        for (size_t i = 0; i < shapes_.size(); ++i) {
            shapes_[i]->print(os);
            if (i != shapes_.size() - 1) os << ",\n";
        }
        os << "]";
    }
    Bounds getBounds() const override {
        if (shapes_.empty()) {
            throw std::logic_error("CompositeShape::getBounds: no shapes");
        }
        Bounds total = shapes_[0]->getBounds();
        for (size_t i = 1; i < shapes_.size(); ++i) {
            Bounds b = shapes_[i]->getBounds();
            total.left = std::min(total.left, b.left);
            total.bottom = std::min(total.bottom, b.bottom);
            total.right = std::max(total.right, b.right);
            total.top = std::max(total.top, b.top);
        }
        return total;
    }
};

#endif
