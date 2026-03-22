#include "composite_shape.h"
#include <stdexcept>
#include <utility>

CompositeShape::CompositeShape() : shapes_(nullptr), size_(0), capacity_(0) {}

CompositeShape::CompositeShape(const CompositeShape& other) : 
    shapes_(new Shape*[other.capacity_]), size_(other.size_), capacity_(other.capacity_) {
    for (size_t i = 0; i < size_; ++i) {
        shapes_[i] = other.shapes_[i]->clone();
    }
}

CompositeShape::CompositeShape(CompositeShape&& other) noexcept : 
    shapes_(other.shapes_), size_(other.size_), capacity_(other.capacity_) {
    other.shapes_ = nullptr;
    other.size_ = 0;
    other.capacity_ = 0;
}

void CompositeShape::swap(CompositeShape& other) noexcept {
    std::swap(shapes_, other.shapes_);
    std::swap(size_, other.size_);
    std::swap(capacity_, other.capacity_);
}

CompositeShape& CompositeShape::operator=(const CompositeShape& other) {
    if (this != &other) {
        CompositeShape temp(other);
        swap(temp);
    }
    return *this;
}

CompositeShape& CompositeShape::operator=(CompositeShape&& other) noexcept {
    if (this != &other) {
        for (size_t i = 0; i < size_; ++i) {
            delete shapes_[i];
        }
        delete[] shapes_;
        shapes_ = other.shapes_;
        size_ = other.size_;
        capacity_ = other.capacity_;
        other.shapes_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }
    return *this;
}

CompositeShape::~CompositeShape() {
    for (size_t i = 0; i < size_; ++i) {
        delete shapes_[i];
    }
    delete[] shapes_;
}

void CompositeShape::addShape(const Shape* shape) {
    if (!shape) {
        throw std::invalid_argument("Nullptr shape");
    }
    if (size_ == capacity_) {
        size_t newCapacity = (capacity_ == 0) ? 2 : capacity_ * 2;
        Shape** newShapes = new Shape*[newCapacity];
        for (size_t i = 0; i < size_; ++i) {
            newShapes[i] = shapes_[i];
        }
        delete[] shapes_;
        shapes_ = newShapes;
        capacity_ = newCapacity;
    }
    shapes_[size_++] = shape->clone();
}

double CompositeShape::getArea() const {
    double area = 0.0;
    for (size_t i = 0; i < size_; ++i) {
        area += shapes_[i]->getArea();
    }
    return area;
}

Point CompositeShape::getCenter() const {
    if (size_ == 0) {
        return {0.0, 0.0};
    }
    double sumX = 0.0;
    double sumY = 0.0;
    for (size_t i = 0; i < size_; ++i) {
        sumX += shapes_[i]->getCenter().x;
        sumY += shapes_[i]->getCenter().y;
    }
    return {sumX / size_, sumY / size_};
}

void CompositeShape::move(double dx, double dy) {
    for (size_t i = 0; i < size_; ++i) {
        shapes_[i]->move(dx, dy);
    }
}

void CompositeShape::scale(double coefficient) {
    if (coefficient <= 0.0) {
        throw std::invalid_argument("Invalid coefficient");
    }
    Point center = getCenter();
    for (size_t i = 0; i < size_; ++i) {
        shapes_[i]->scale(coefficient);
        Point shapeCenter = shapes_[i]->getCenter();
        double dx = (shapeCenter.x - center.x) * (coefficient - 1.0);
        double dy = (shapeCenter.y - center.y) * (coefficient - 1.0);
        shapes_[i]->move(dx, dy);
    }
}

std::string CompositeShape::getName() const {
    return "COMPOSITE";
}

Shape* CompositeShape::clone() const {
    return new CompositeShape(*this);
}