#include <iostream>
#include <iomanip>
#include <vector>
#include <memory>
#include "Rectangle.h"
#include "Circle.h"
#include "RectangularTrapezoid.h"
#include "CompositeShape.h"

void printAll(const std::vector<std::unique_ptr<Shape>>& shapes) {
    for (const std::unique_ptr<Shape>& shape : shapes) {
        shape->print(std::cout);
        std::cout << std::endl;
    }
}

int main() {
    std::cout << std::fixed << std::setprecision(2);

    std::vector<std::unique_ptr<Shape>> shapes;

    shapes.push_back(std::make_unique<Rectangle>(Point(0, 0), Point(4, 3)));
    shapes.push_back(std::make_unique<Circle>(Point(5, 5), 2.0));
    shapes.push_back(std::make_unique<RectangularTrapezoid>(Point(1, 1), 6.0, 4.0, 3.0));
    shapes.push_back(std::make_unique<Circle>(Point(-2, -2), 1.5));

    std::unique_ptr<CompositeShape> composite = std::make_unique<CompositeShape>();
    composite->addShape(std::make_unique<Rectangle>(Point(10, 10), Point(12, 15)));
    composite->addShape(std::make_unique<Circle>(Point(14, 12), 1.0));
    composite->addShape(std::make_unique<RectangularTrapezoid>(Point(9, 8), 5.0, 3.0, 2.0));
    shapes.push_back(std::move(composite));

    std::cout << "BEFORE SCALING" << "\n";
    printAll(shapes);

    for (std::unique_ptr<Shape>& shape : shapes) {
        shape->scale(2.0);
    }

    std::cout << "\n" << "AFTER SCALING" << "\n";
    printAll(shapes);

    return 0;
}
