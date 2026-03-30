#include "rectangle.hpp"
#include "circle.hpp"
#include "right_trapezoid.hpp"
#include "composite_shape.hpp"
#include <iostream>

int main() {
    try {
        std::cout << "Simple Shapes\n";

        Rectangle rect(Point(0, 0), 4, 3);
        std::cout << "Rectangle: " << rect << std::endl;
        std::cout << "Area: " << rect.area() << std::endl;

        Circle circle(Point(5, 5), 2);
        std::cout << "\nCircle: " << circle << std::endl;
        std::cout << "Area: " << circle.area() << std::endl;

        RightTrapezoid trapezoid(Point(2, 1), 6, 4, 3);
        std::cout << "\nRight Trapezoid: " << trapezoid << std::endl;
        std::cout << "Area: " << trapezoid.area() << std::endl;

        std::cout << "\nComposite Shape\n";

        CompositeShape composite;
        composite.addShape<Rectangle>(Point(0, 0), 4, 3);
        composite.addShape<Circle>(Point(5, 5), 2);
        composite.addShape<RightTrapezoid>(Point(2, 1), 6, 4, 3);

        std::cout << "Composite Shape:\n" << composite << std::endl;
        std::cout << "Total Area: " << composite.area() << std::endl;

        std::cout << "\nScaling x2\n";
        composite.scale(2.0);
        std::cout << "After scaling:\n" << composite << std::endl;

    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
