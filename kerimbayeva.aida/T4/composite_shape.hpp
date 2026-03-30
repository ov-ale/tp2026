#ifndef COMPOSITE_SHAPE_HPP
#define COMPOSITE_SHAPE_HPP

#include "shape.hpp"
#include <vector>
#include <memory>

class CompositeShape : public Shape {
private:
    std::vector<std::unique_ptr<Shape>> shapes;

    void getBoundingBox(Point& bottomLeft, Point& topRight) const;

public:
    CompositeShape() = default;

    CompositeShape(const CompositeShape&) = delete;
    CompositeShape& operator=(const CompositeShape&) = delete;

    CompositeShape(CompositeShape&&) = default;
    CompositeShape& operator=(CompositeShape&&) = default;

    template<typename T, typename... Args>
    void addShape(Args&&... args) {
        shapes.push_back(std::make_unique<T>(std::forward<Args>(args)...));
    }

    void addShape(std::unique_ptr<Shape> shape);

    double area() const override;
    Point center() const override;
    void scale(double factor) override;
    void print(std::ostream& os) const override;

    size_t size() const { return shapes.size(); }
    const Shape* getShape(size_t index) const;
};

#endif
