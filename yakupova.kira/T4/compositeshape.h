#ifndef COMPOSITESHAPE_H
#define COMPOSITESHAPE_H

#include <memory>
#include <vector>
#include "shape.h"

class CompositeShape : public Shape {
public:
    CompositeShape() = default;

    void addShape(std::unique_ptr<Shape> shape);

    double getArea() const override;
    Point getCenter() const override;
    void move(double dx, double dy) override;
    void scale(double factor) override;
    std::string getName() const override;

    size_t getCount() const;
    const Shape& getShapeAt(size_t index) const;

private:
    std::vector<std::unique_ptr<Shape>> shapes_;
};

#endif
