#include "rectangle.hpp"
#include <iomanip>
#include <stdexcept>

Rectangle::Rectangle(const Point& bl, double w, double h)
    : bottomLeft(bl), width(w), height(h) {
    if (w <= 0 || h <= 0) {
        throw std::invalid_argument("Width and height must be positive");
    }
}

double Rectangle::area() const {
    return width * height;
}

Point Rectangle::center() const {
    return Point(bottomLeft.x + width / 2, bottomLeft.y + height / 2);
}

void Rectangle::scale(double factor) {
    if (factor <= 0) {
        throw std::invalid_argument("Scale factor must be positive");
    }

    Point c = center();
    width *= factor;
    height *= factor;
    bottomLeft.x = c.x - width / 2;
    bottomLeft.y = c.y - height / 2;
}

void Rectangle::print(std::ostream& os) const {
    os << std::fixed << std::setprecision(2);
    os << "RECTANGLE, (" << bottomLeft.x << ", " << bottomLeft.y << "), "
        << width << ", " << height;
}
