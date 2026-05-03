#include <iostream>
#include <vector>
#include <memory>
#include <iomanip>
#include "circle.h"
#include "rectangle.h"
#include "isosceles_trapezoid.h"
#include "composite_shape.h"

void printShapesInfo(const std::vector<std::unique_ptr<Shape>>& shapes) {
    std::cout << std::fixed << std::setprecision(2);
    for (const auto& shape : shapes) {
        Point center = shape->getCenter();
        std::cout << "[" << shape->getName() << ", ("
                  << center.x << ", " << center.y << "), "
                  << shape->getArea() << "]" << std::endl;
    }
}
int main() {
    try {
        std::vector<std::unique_ptr<Shape>> shapes;

        shapes.push_back(std::make_unique<Circle>(Point(0, 0), 5.0));
        shapes.push_back(std::make_unique<Circle>(Point(3, 4), 2.5));
        shapes.push_back(std::make_unique<Rectangle>(Point(0, 0), Point(8, 3)));
        shapes.push_back(std::make_unique<Rectangle>(Point(2, 1), Point(8, 3)));
        shapes.push_back(std::make_unique<IsoscelesTrapezoid>(Point(0, 0), 8.0, 4.0, 3.0));
        shapes.push_back(std::make_unique<IsoscelesTrapezoid>(Point(2, 1), 6.0, 3.0, 2.0));

        auto composite = std::make_unique<CompositeShape>();
        composite->addShape(std::make_unique<Circle>(Point(5, 5), 2.0));
        composite->addShape(std::make_unique<Circle>(Point(10, 8), 1.5));
        composite->addShape(std::make_unique<Rectangle>(Point(8, 2), Point(13, 4)));
        composite->addShape(std::make_unique<IsoscelesTrapezoid>(Point(8, 2), 5.0, 3.0, 2.0));

        shapes.push_back(std::move(composite));

        std::cout << "=== BEFORE SCALING ===" << std::endl;
        printShapesInfo(shapes);

        double factor;
        std::cout << "\nEnter scale factor: ";
        std::cin >> factor;

        std::cout << "\n=== BONUS 1: POINT CONTAINS TEST ===" << std::endl;
        Point testPoint(2.5, 1.8);
        std::cout << "Testing point (2.50, 1.80):" << std::endl;
        for (size_t i = 0; i < shapes.size(); ++i) {
            std::cout << " Shape " << i << " (" << shapes[i]->getName() << "): ";
            if (shapes[i]->containsPoint(testPoint)) {
                std::cout << "INSIDE" << std::endl;
            } else {
                std::cout << "OUTSIDE" << std::endl;
            }
        }
        std::cout << "\n=== BONUS 2: PERIMETER TEST ===" << std::endl;
        std::cout << "Perimeter of each shape:" << std::endl;
        for (size_t i = 0; i < shapes.size(); ++i) {
            std::cout << " Shape " << i << " (" << shapes[i]->getName() << "): "
                      << shapes[i]->getPerimeter() << std::endl;
        }
        std::cout << "\n--- SCALING BY FACTOR " << std::fixed << std::setprecision(1)
                  << factor << " ---" << std::endl;

        for (auto& shape : shapes) {
            shape->scale(factor);
        }
        std::cout << "\n=== AFTER SCALING ===" << std::endl;
        printShapesInfo(shapes);

        std::cout << "\n=== BONUS 1: POINT CONTAINS TEST (AFTER SCALING) ===" << std::endl;
        std::cout << "Testing point (2.50, 1.80) after scaling:" << std::endl;
        for (size_t i = 0; i < shapes.size(); ++i) {
            std::cout << " Shape " << i << " (" << shapes[i]->getName() << "): ";
            if (shapes[i]->containsPoint(testPoint)) {
                std::cout << "INSIDE" << std::endl;
            } else {
                std::cout << "OUTSIDE" << std::endl;
            }
        }
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
//

