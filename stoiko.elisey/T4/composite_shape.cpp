#include "composite_shape.h"

void CompositeShape::addShape(std::unique_ptr<Shape> shape) {
    if (shape) {
        shapes.push_back(std::move(shape));
    }
}

double CompositeShape::getArea() const {
    double area = 0.0;
    for (int i = 0; i < shapes.size(); ++i) {
        area += shapes[i]->getArea();
    }
    return area;
}

Point CompositeShape::getCenter() const {
    double accX = 0, accY = 0;
    for (int i = 0; i < shapes.size(); ++i) {
        accX += shapes[i]->getCenter().x;
        accY += shapes[i]->getCenter().y;
    }
    return Point(accX / shapes.size(), accY / shapes.size());
}

void CompositeShape::move(double dx, double dy) {
    for (int i = 0; i < shapes.size(); ++i) {
        shapes[i]->move(dx, dy);
    }
}

void CompositeShape::scale(double factor) {
    Point compositeCenter = getCenter();

    for (int i = 0; i < shapes.size(); ++i) {
        Point shapeCenter = shapes[i]->getCenter();

        Point newShapeCenter(
            compositeCenter.x + (shapeCenter.x - compositeCenter.x) * factor,
            compositeCenter.y + (shapeCenter.y - compositeCenter.y) * factor
        );

        shapes[i]->move(
            newShapeCenter.x - shapeCenter.x,
            newShapeCenter.y - shapeCenter.y
        );

        shapes[i]->scale(factor);
    }
}

std::string CompositeShape::getName() const {
    return "COMPOSITE";
}
