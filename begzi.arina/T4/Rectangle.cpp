#include "Rectangle.h"

Rectangle::Rectangle(Point bottom_left, Point top_right) : bl(bottom_left), tr(top_right) {}
double Rectangle::getArea() const { return (tr.x - bl.x) * (tr.y - bl.y); }
Point Rectangle::getCenter() const { return { (bl.x + tr.x) / 2.0, (bl.y + tr.y) / 2.0 }; }
void Rectangle::move(double dx, double dy) { bl.x += dx; tr.x += dx; bl.y += dy; tr.y += dy; }
void Rectangle::scale(double f) {
    Point c = getCenter();
    bl.x = c.x + (bl.x - c.x) * f; bl.y = c.y + (bl.y - c.y) * f;
    tr.x = c.x + (tr.x - c.x) * f; tr.y = c.y + (tr.y - c.y) * f;
}
std::string Rectangle::getName() const { return "RECTANGLE"; }
void Rectangle::getBoundingBox(Point& b, Point& t) const { b = bl; t = tr; }

