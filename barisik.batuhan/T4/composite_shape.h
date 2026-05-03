#ifndef COMPOSITE_SHAPE_H
#define COMPOSITE_SHAPE_H

#include "shape.h"
#include <memory>
#include <vector>

class CompositeShape : public Shape {
private:
    std::vector<std::unique_ptr<Shape>> shapes_;

public:
    CompositeShape() = default;
    CompositeShape(const CompositeShape&) = delete;
    CompositeShape& operator=(const CompositeShape&) = delete;
    CompositeShape(CompositeShape&&) = default;
    CompositeShape& operator=(CompositeShape&&) = default;

    void addShape(std::unique_ptr<Shape> shape);
    
    const std::vector<std::unique_ptr<Shape>>& getShapes() const {
        return shapes_;
    }
    double getArea() const override;
    Point getCenter() const override;
    void move(double dx, double dy) override;
    void scale(double factor) override;
    std::string getName() const override;
    bool containsPoint(const Point& p) const override;
    double getPerimeter() const override;
    void getBoundingBox(Point& min, Point& max) const override;
};

#endif
//

