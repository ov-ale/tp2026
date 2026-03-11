#include <iostream>
#include <iomanip>
#include <memory>
#include <vector>
#include <cstring>
#include "rectangle.h"
#include "ring.h"
#include "rhombus.h"
#include "composite_shape.h"

void printShapeInfo(const Shape& shape) {
    Point center = shape.getCenter();
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "[" << shape.getName() << ", (" << center.x << ", " << center.y << "), " << shape.getArea() << "]" << std::endl;
}

void printUsage() {
    std::cerr << "Usage: program [--scale <factor>]" << std::endl;
    std::cerr << "Without arguments: shows shapes without scaling" << std::endl;
}

void demonstrateShapes() {
    std::cout << "Creating shapes" << std::endl;
    
    Rectangle rect(Point(0, 0), Point(4, 3));
    Ring ring(Point(5, 5), 5.0, 2.0);
    Rhombus rhombus(Point(10, 10), 6.0, 4.0);
    
    std::vector<std::unique_ptr<Shape>> shapes;
    shapes.push_back(std::make_unique<Rectangle>(Point(1, 1), Point(3, 4)));
    shapes.push_back(std::make_unique<Ring>(Point(-2, -2), 3.0, 1.0));
    shapes.push_back(std::make_unique<Rhombus>(Point(2, -3), 5.0, 3.0));
    
    CompositeShape composite;
    composite.addShape(std::make_unique<Rectangle>(Point(2, 2), Point(5, 5)));
    composite.addShape(std::make_unique<Ring>(Point(3, 3), 2.0, 0.5));
    composite.addShape(std::make_unique<Rhombus>(Point(4, 4), 4.0, 2.0));
    
    std::cout << "\nShape information WITHOUT scaling" << std::endl;
    
    std::cout << "\nIndividual shapes:" << std::endl;
    printShapeInfo(rect);
    printShapeInfo(ring);
    printShapeInfo(rhombus);
    
    std::cout << "\nShapes in vector:" << std::endl;
    for (const auto& shape : shapes) {
        printShapeInfo(*shape);
    }
    
    std::cout << "\nComposite shape:" << std::endl;
    composite.printInfo();
}

void demonstrateScaling(double scaleFactor) {
    Rectangle rect(Point(0, 0), Point(4, 3));
    Ring ring(Point(5, 5), 5.0, 2.0);
    Rhombus rhombus(Point(10, 10), 6.0, 4.0);
    
    std::cout << "Creating shapes" << std::endl;
    std::cout << "\nShape information BEFORE scaling" << std::endl;
    
    std::cout << "\nIndividual shapes:" << std::endl;
    printShapeInfo(rect);
    printShapeInfo(ring);
    printShapeInfo(rhombus);
    
    rect.scale(scaleFactor);
    ring.scale(scaleFactor);
    rhombus.scale(scaleFactor);
    
    std::cout << "\nShape information AFTER scaling by " << scaleFactor << std::endl;
    
    std::cout << "\nIndividual shapes:" << std::endl;
    printShapeInfo(rect);
    printShapeInfo(ring);
    printShapeInfo(rhombus);
}

int main(int argc, char* argv[]) {
    try {
        if (argc == 1) {
            // Нет аргументов - показываем фигуры без масштабирования
            demonstrateShapes();
            return 0;
        } else if (argc == 3 && std::strcmp(argv[1], "--scale") == 0) {
            double scaleFactor = std::stod(argv[2]);
            demonstrateScaling(scaleFactor);
            return 0;
        } else {
            std::cerr << "Error: Invalid arguments" << std::endl;
            printUsage();
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        return 1;
    }
}
