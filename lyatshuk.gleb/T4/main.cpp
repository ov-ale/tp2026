#include <iostream>
#include <iomanip>
#include <memory>
#include <vector>
#include "rectangle.h"
#include "ring.h"
#include "rhombus.h"
#include "composite_shape.h"

void printShapeInfo(const Shape& shape) {
    Point center = shape.getCenter();
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "[" << shape.getName() << ", (" << center.x << ", " << center.y << "), " << shape.getArea() << "]" << std::endl;
}

int main() {
    try {
        std::cout << "Creating shapes" << std::endl;
        
        // Create individual shapes
        Rectangle rect(Point(0, 0), Point(4, 3));
        Ring ring(Point(5, 5), 5.0, 2.0);
        Rhombus rhombus(Point(10, 10), 6.0, 4.0);
        
        std::vector<std::unique_ptr<Shape>> shapes;
        shapes.push_back(std::make_unique<Rectangle>(Point(1, 1), Point(3, 4)));
        shapes.push_back(std::make_unique<Ring>(Point(-2, -2), 3.0, 1.0));
        shapes.push_back(std::make_unique<Rhombus>(Point(2, -3), 5.0, 3.0));
        
        // Create composite shape
        CompositeShape composite;
        composite.addShape(std::make_unique<Rectangle>(Point(2, 2), Point(5, 5)));
        composite.addShape(std::make_unique<Ring>(Point(3, 3), 2.0, 0.5));
        composite.addShape(std::make_unique<Rhombus>(Point(4, 4), 4.0, 2.0));
        
        std::cout << "\nShape information BEFORE scaling" << std::endl;
        
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
        
        // Scale all shapes by factor 2
        double scaleFactor = 2.0;
        rect.scale(scaleFactor);
        ring.scale(scaleFactor);
        rhombus.scale(scaleFactor);
        
        for (auto& shape : shapes) {
            shape->scale(scaleFactor);
        }
        
        composite.scale(scaleFactor);
        
        std::cout << "\nShape information AFTER scaling by 2" << std::endl;
        
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
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
