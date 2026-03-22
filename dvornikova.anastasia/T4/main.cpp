#include <iostream>
#include <iomanip>
#include <vector>
#include "rectangle.h"
#include "ellipse.h"
#include "square.h"
#include "composite_shape.h"

void printInfo(const Shape* shape) {
    std::cout << "[" << shape->getName() << ", ("
              << std::fixed << std::setprecision(2) << shape->getCenter().x << ", "
              << std::fixed << std::setprecision(2) << shape->getCenter().y << "), "
              << std::fixed << std::setprecision(2) << shape->getArea() << "]\n";
}

int main() {
    Rectangle rect1({0.0, 0.0}, {4.0, 3.0});
    Rectangle rect2({-1.0, -1.0}, {1.0, 1.0});
    Ellipse el({2.0, 2.0}, 3.0, 2.0);
    Square sq({1.0, 1.0}, 2.0);

    CompositeShape comp;
    comp.addShape(&rect1);
    comp.addShape(&el);

    std::vector<Shape*> shapes = {&rect1, &rect2, &el, &sq, &comp};

    for (const auto* shape : shapes) {
        printInfo(shape);
    }

    std::cout << "\n";

    for (auto* shape : shapes) {
        shape->scale(2.0);
        printInfo(shape);
    }

    return 0;
}