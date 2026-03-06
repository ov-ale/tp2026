#ifndef COMPOSITE_SHAPE_H
#define COMPOSITE_SHAPE_H

#include "point.h"
#include "shape.h"
#include <memory>
#include <string>
#include <vector>

class CompositeShape : public Shape {
private:
    std::vector<std::shared_ptr<Shape>> shapes;
public:
    void addShape(std::unique_ptr<Shape> shape);
    double getArea() const override;
    Point getCenter() const override;
    void move(double dx, double dy) override;
    void scale(double factor) override;
    std::string getName() const override;
};

#endif // COMPOSITE_SHAPE_H
