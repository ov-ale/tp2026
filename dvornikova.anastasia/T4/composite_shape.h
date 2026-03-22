#ifndef COMPOSITE_SHAPE_H
#define COMPOSITE_SHAPE_H

#include "shape.h"

class CompositeShape : public Shape {
public:
    CompositeShape();
    CompositeShape(const CompositeShape& other);
    CompositeShape(CompositeShape&& other) noexcept;
    CompositeShape& operator=(const CompositeShape& other);
    CompositeShape& operator=(CompositeShape&& other) noexcept;
    ~CompositeShape() override;

    void addShape(const Shape* shape);
    
    double getArea() const override;
    Point getCenter() const override;
    void move(double dx, double dy) override;
    void scale(double coefficient) override;
    std::string getName() const override;
    Shape* clone() const override;

private:
    Shape** shapes_;
    size_t size_;
    size_t capacity_;
    void swap(CompositeShape& other) noexcept;
};

#endif