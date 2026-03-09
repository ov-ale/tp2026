#include "Shape.h"
#include <iostream>
#include <iomanip>

void Shape::print() const {
    Point c = getCenter();
    std::cout << "[" << getName() << ", ("
        << std::fixed << std::setprecision(2) << c.x << ", " << c.y
        << "), " << getArea() << "]";
}
