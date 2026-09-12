#include <iomanip>
#include <iostream>
#include <memory>
#include <vector>
#include "compositeshape.h"
#include "ellipse.h"
#include "point.h"
#include "rectangle.h"
#include "rhombus.h"
#include "shape.h"

// Печатает фигуру в одну строку без квадратных скобок: NAME, (x.xx, y.xx), area.xx
void printShapeInline(const Shape& shape, std::ostream& out) {
    Point c = shape.getCenter();
    out << shape.getName() << ", ("
        << std::fixed << std::setprecision(2) << c.x << ", " << c.y << "), "
        << shape.getArea();
}

// Печатает фигуру целиком; для составной фигуры дополнительно печатает
// все вложенные фигуры с отступом.
void printShape(const Shape& shape, std::ostream& out) {
    const CompositeShape* composite = dynamic_cast<const CompositeShape*>(&shape);

    out << "[";
    printShapeInline(shape, out);

    if (composite) {
        out << ":\n";
        for (size_t i = 0; i < composite->getCount(); ++i) {
            out << "  ";
            printShapeInline(composite->getShapeAt(i), out);
            out << ",\n";
        }
    }

    out << "]\n";
}

void printAll(const std::vector<std::unique_ptr<Shape>>& shapes, std::ostream& out) {
    for (const auto& shape : shapes) {
        printShape(*shape, out);
    }
}

int main() {
    std::vector<std::unique_ptr<Shape>> shapes;

    shapes.push_back(std::unique_ptr<Shape>(
        new Rectangle(Point{0.0, 0.0}, Point{4.0, 2.0})));
    shapes.push_back(std::unique_ptr<Shape>(
        new Ellipse(Point{5.0, 5.0}, 3.0, 2.0)));
    shapes.push_back(std::unique_ptr<Shape>(
        new Rhombus(Point{-3.0, 1.0}, 4.0, 6.0)));

    std::unique_ptr<CompositeShape> composite(new CompositeShape());
    composite->addShape(std::unique_ptr<Shape>(
        new Rectangle(Point{10.0, 10.0}, Point{12.0, 12.0})));
    composite->addShape(std::unique_ptr<Shape>(
        new Ellipse(Point{15.0, 10.0}, 1.0, 1.0)));
    shapes.push_back(std::move(composite));

    shapes.push_back(std::unique_ptr<Shape>(
        new Rectangle(Point{-5.0, -5.0}, Point{-1.0, -1.0})));

    std::cout << "До увеличения:\n";
    printAll(shapes, std::cout);

    for (auto& shape : shapes) {
        shape->scale(2.0);
    }

    std::cout << "\nПосле увеличения в 2 раза:\n";
    printAll(shapes, std::cout);

    return 0;
}
