#include <iostream>
#include <vector>
#include <memory>
#include "point.h"
#include "rectangle.h"
#include "ring.h"
#include "square.h"
#include "composite_shape.h"

int main() {
    std::vector<std::unique_ptr<Shape>> shapes;

    shapes.push_back(std::make_unique<Rectangle>(
        Point(1.0, 1.0), Point(4.0, 3.0)
    ));

    shapes.push_back(std::make_unique<Ring>(
        Point(5.0, 5.0), 3.0, 1.0
    ));

    shapes.push_back(std::make_unique<Square>(
        Point(3.0, 3.0), 3.0
    ));

    auto composite = std::make_unique<CompositeShape>();
    composite->addShape(std::make_unique<Rectangle>(Point(10.0, 10.0), Point(13.0, 12.0)));
    composite->addShape(std::make_unique<Square>(Point(15.0, 10.0), 2.0));
    composite->addShape(std::make_unique<Ring>(Point(11.0, 15.0), 2.0, 1.0));

    shapes.push_back(std::move(composite));

    std::cout << "BEFORE SCALING BY 2:\n";
    std::cout << "====================\n";
    for (const auto& shape : shapes) {
        shape->print();
        std::cout << "\n";
    }

    std::cout << "\nSCALING BY 2...\n\n";
    for (auto& shape : shapes) {
        shape->scale(2.0);
    }

    std::cout << "AFTER SCALING BY 2:\n";
    std::cout << "===================\n";
    for (const auto& shape : shapes) {
        shape->print();
        std::cout << "\n";
    }

    return 0;
}
