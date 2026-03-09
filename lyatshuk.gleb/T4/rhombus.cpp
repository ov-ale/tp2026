#include "rhombus.h"
#include <cmath>
#include <stdexcept>

Rhombus::Rhombus(const Point& center, double verticalDiagonal, double horizontalDiagonal)
    : center(center), verticalDiagonal(verticalDiagonal), horizontalDiagonal(horizontalDiagonal) {
    if (verticalDiagonal <= 0 || horizontalDiagonal <= 0) {
        throw std::invalid_argument("Diagonals must be positive");
    }
}

double Rhombus::getArea() const {
    return (verticalDiagonal * horizontalDiagonal) / 2.0;
}

Point Rhombus::getCenter() const {
    return center;
}

void Rhombus::move(double dx, double dy) {
    center.x += dx;
    center.y += dy;
}

void Rhombus::scale(double factor) {
    verticalDiagonal *= factor;
    horizontalDiagonal *= factor;
}

std::string Rhombus::getName() const {
    return "RHOMBUS";
}
