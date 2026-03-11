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

void Rhombus::getBounds(double& minX, double& minY, double& maxX, double& maxY) const {
    minX = center.x - horizontalDiagonal / 2;
    minY = center.y - verticalDiagonal / 2;
    maxX = center.x + horizontalDiagonal / 2;
    maxY = center.y + verticalDiagonal / 2;
}
