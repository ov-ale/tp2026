#include <iostream>
#include <iomanip>
#include <memory>
#include <vector>
#include "rectangle.h"
#include "ellipse.h"
#include "rhombus.h"
#include "composite_shape.h"

void printShapeInfo(const Shape* shape) {
    Point center = shape->getCenter();
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "[" << shape->getName() << ", ("
              << center.x << ", " << center.y << "), "
              << shape->getArea() << "]";

    const CompositeShape* composite = dynamic_cast<const CompositeShape*>(shape);
    if (composite) {
        std::cout << ":" << std::endl;
        composite->printComponents();
    } else {
        std::cout << std::endl;
    }
}

void printAllShapes(const std::vector<std::unique_ptr<Shape>>& shapes, const std::string& title) {
    std::cout << title << ":" << std::endl;
    for (const auto& shape : shapes) {
        printShapeInfo(shape.get());
    }
    std::cout << std::endl;
}

int main() {
    std::vector<std::unique_ptr<Shape>> shapes;

    shapes.push_back(std::make_unique<Rectangle>(Point(0, 0), Point(4, 3)));
    shapes.push_back(std::make_unique<Ellipse>(Point(2, 5), 3, 2));
    shapes.push_back(std::make_unique<Rhombus>(Point(8, 2), 6, 4));

    auto composite = std::make_unique<CompositeShape>();

    composite->addShape(std::make_unique<Rectangle>(Point(10, 10), Point(15, 13)));
    composite->addShape(std::make_unique<Ellipse>(Point(12, 15), 2, 1.5));
    composite->addShape(std::make_unique<Rhombus>(Point(18, 12), 4, 3));

    shapes.push_back(std::move(composite));

    shapes.push_back(std::make_unique<Rectangle>(Point(5, 8), Point(7, 10)));

    printAllShapes(shapes, "Фигуры до масштабирования");

    for (auto& shape : shapes) {
        shape->scale(2.0);
    }

    printAllShapes(shapes, "Фигуры после масштабирования (x2)");

    return 0;
}
