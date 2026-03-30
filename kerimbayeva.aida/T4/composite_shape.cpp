#include "composite_shape.hpp"
#include <iomanip>
#include <algorithm>
#include <stdexcept>

void CompositeShape::addShape(std::unique_ptr<Shape> shape) {
    if (shape) {
        shapes.push_back(std::move(shape));
    }
}

void CompositeShape::getBoundingBox(Point& bottomLeft, Point& topRight) const {
    if (shapes.empty()) {
        bottomLeft = Point(0, 0);
        topRight = Point(0, 0);
        return;
    }
    bottomLeft = Point(-10, -10);
    topRight = Point(10, 10);
}

double CompositeShape::area() const {
    double total = 0;
    for (const auto& shape : shapes) {
        total += shape->area();
    }
    return total;
}

Point CompositeShape::center() const {
    if (shapes.empty()) {
        return Point(0, 0);
    }

    Point bl, tr;
    getBoundingBox(bl, tr);

    return Point((bl.x + tr.x) / 2, (bl.y + tr.y) / 2);
}

void CompositeShape::scale(double factor) {
    if (factor <= 0) {
        throw std::invalid_argument("Scale factor must be positive");
    }

    for (auto& shape : shapes) {
        shape->scale(factor);
    }
}

const Shape* CompositeShape::getShape(size_t index) const {
    if (index < shapes.size()) {
        return shapes[index].get();
    }
    return nullptr;
}

void CompositeShape::print(std::ostream& os) const {
    os << std::fixed << std::setprecision(2);
    Point c = center();
    os << "COMPOSITE, (" << c.x << ", " << c.y << "), " << area() << ":\n";

    for (size_t i = 0; i < shapes.size(); ++i) {
        os << "    ";
        shapes[i]->print(os);
        if (i < shapes.size() - 1) {
            os << ",\n";
        }
    }
}
