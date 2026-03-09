#include "Circle.h"

Circle::Circle(Point center, double radius) : pos(center), r(radius) {}
double Circle::getArea() const { return 3.14159265 * r * r; }
Point Circle::getCenter() const { return pos; }
void Circle::move(double dx, double dy) { pos.x += dx; pos.y += dy; }
void Circle::scale(double f) { r *= f; }
std::string Circle::getName() const { return "CIRCLE"; }
void Circle::getBoundingBox(Point& bl, Point& tr) const {
    bl = { pos.x - r, pos.y - r }; tr = { pos.x + r, pos.y + r };
}