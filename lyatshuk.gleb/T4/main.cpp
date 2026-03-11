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
    std::cerr << "If no arguments provided, demonstrates shapes with scaling by 2" << std::endl;
}

int main(int argc, char* argv[]) {
    try {
        if (argc > 1) {
            if (argc == 3 && std::strcmp(argv[1], "--scale") == 0) {
                double scaleFactor = std::stod(argv[2]);
                
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
                
                return 0;
            } else {
                std::cerr << "Error: Invalid arguments" << std::endl;
                printUsage();
                return 1;
            }
        } else {
            std::cerr << "Error: No arguments provided. Use --scale <factor> to scale shapes." << std::endl;
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
