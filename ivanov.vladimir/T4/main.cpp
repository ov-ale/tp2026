#include <iostream>
#include <iomanip>
#include <vector>
#include <memory>
#include "rectangle.h"
#include "circle.h"
#include "composite-shape.h"

void printShape(const Shape &shape) {
    Point c = shape.getCenter();
    const CompositeShape *comp = dynamic_cast<const CompositeShape *>(&shape);
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
        auto r1 = std::make_shared<Rectangle>(Point{0, 0}, Point{2, 2});
        auto r2 = std::make_shared<Rectangle>(Point{5, 5}, Point{10, 10});
        auto c1 = std::make_shared<Circle>(Point{15, 15}, 5);
        auto c2 = std::make_shared<Circle>(Point{-5, -5}, 3);
        auto comp = std::make_shared<CompositeShape>();
        comp->addShape(r1);
        comp->addShape(c1);

        std::vector<std::shared_ptr<Shape> > shapes = {r2, c2, comp};
        shapes.push_back(std::make_shared<Circle>(Point{1, 1}, 1));
        shapes.push_back(std::make_shared<Rectangle>(Point{-2, -2}, Point{-1, -1}));

        for (const auto &s: shapes) { printShape(*s); }
        for (auto &s: shapes) { s->scale(2.0); }
        for (const auto &s: shapes) { printShape(*s); }
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
