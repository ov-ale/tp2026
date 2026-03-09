#include <iostream>
#include <iomanip>
#include <vector>
#include <memory>
#include "rectangle.h"
#include "circle.h"
#include "rectangle-trapezoid.h"
#include "composite-shape.h"

void printShape(const Shape& shape) {
    Point c = shape.getCenter();
    const CompositeShape* comp = dynamic_cast<const CompositeShape*>(&shape);
    if (comp) {
        std::cout << "[COMPOSITE, (" << std::fixed << std::setprecision(2)
                  << c.x << ", " << c.y << "), " << shape.getArea() << ":\n";
        for (size_t i = 0; i < comp->getSize(); ++i) {
            std::shared_ptr<Shape> inner = comp->getShape(i);
            Point ic = inner->getCenter();
            std::cout << inner->getName() << ", (" << ic.x << ", " << ic.y
                      << "), " << inner->getArea() << ",\n";
        }
        std::cout << "]\n";
    } else {
        std::cout << "[" << shape.getName() << ", (" << std::fixed << std::setprecision(2)
                  << c.x << ", " << c.y << "), " << shape.getArea() << "]\n";
    }
}

int main() {
    try {
        auto r = std::make_shared<Rectangle>(Point{0, 0}, Point{2, 2});
        auto c = std::make_shared<Circle>(Point{15, 15}, 5);
        auto t = std::make_shared<RectangleTrapezoid>(Point{0, 0}, 4, 2, 2);

        auto comp = std::make_shared<CompositeShape>();
        comp->addShape(r);
        comp->addShape(c);
        comp->addShape(t);

        std::vector<std::shared_ptr<Shape>> shapes = {r, c, t, comp};
        shapes.push_back(std::make_shared<Rectangle>(Point{-2, -2}, Point{-1, -1}));

        for (const auto& s : shapes) { printShape(*s); }
        for (auto& s : shapes) { s->scale(2.0); }
        for (const auto& s : shapes) { printShape(*s); }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}