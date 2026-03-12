#include "compositeshape.h"
#include <iostream>

CompositeShape::CompositeShape() {
    capacity = 10;
    count = 0;
    shapes = new Shape*[capacity];
}

CompositeShape::~CompositeShape() {
    for (int i = 0; i < count; i++) {
        delete shapes[i];
    }
    delete[] shapes;
}

void CompositeShape::addShape(Shape* shape) {
    if (count >= capacity) {
        capacity *= 2;
        Shape** newShapes = new Shape*[capacity];
        for (int i = 0; i < count; i++) {
            newShapes[i] = shapes[i];
        }
        delete[] shapes;
        shapes = newShapes;
    }
    shapes[count] = shape;
    count++;
}

double CompositeShape::getArea() {
    double total = 0;
    for (int i = 0; i < count; i++) {
        total += shapes[i]->getArea();
    }
    return total;
}

Point CompositeShape::getCenter() {
    if (count == 0) return Point(0, 0);

    double minX = shapes[0]->getCenter().x;
    double minY = shapes[0]->getCenter().y;
    double maxX = shapes[0]->getCenter().x;
    double maxY = shapes[0]->getCenter().y;

    for (int i = 1; i < count; i++) {
        Point p = shapes[i]->getCenter();
        if (p.x < minX) minX = p.x;
        if (p.y < minY) minY = p.y;
        if (p.x > maxX) maxX = p.x;
        if (p.y > maxY) maxY = p.y;
    }

    return Point((minX + maxX) / 2, (minY + maxY) / 2);
}

void CompositeShape::move(double dx, double dy) {
    for (int i = 0; i < count; i++) {
        shapes[i]->move(dx, dy);
    }
}

void CompositeShape::scale(double factor) {
    Point center = getCenter();

    for (int i = 0; i < count; i++) {
        Point shapeCenter = shapes[i]->getCenter();

        double dx = shapeCenter.x - center.x;
        double dy = shapeCenter.y - center.y;

        shapes[i]->move(dx * (factor - 1), dy * (factor - 1));

        shapes[i]->scale(factor);
    }
}

std::string CompositeShape::getName() {
    return "COMPOSITE";
}
