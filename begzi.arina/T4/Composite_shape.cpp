#include "Composite_shape.h"
#include <iostream>
#include <algorithm>

void CompositeShape::add(std::unique_ptr<Shape> s) { if (s) shapes.push_back(std::move(s)); }
double CompositeShape::getArea() const {
    double total = 0;
    for (const auto& s : shapes) total += s->getArea();
    return total;
}
void CompositeShape::getBoundingBox(Point& minP, Point& maxP) const {
    if (shapes.empty()) { minP = { 0,0 }; maxP = { 0,0 }; return; }
    shapes[0]->getBoundingBox(minP, maxP);
    for (const auto& s : shapes) {
        Point bl, tr; s->getBoundingBox(bl, tr);
        minP.x = std::min(minP.x, bl.x); minP.y = std::min(minP.y, bl.y);
        maxP.x = std::max(maxP.x, tr.x); maxP.y = std::max(maxP.y, tr.y);
    }
}
Point CompositeShape::getCenter() const {
    Point bl, tr; getBoundingBox(bl, tr);
    return { (bl.x + tr.x) / 2.0, (bl.y + tr.y) / 2.0 };
}
void CompositeShape::move(double dx, double dy) {
    for (auto& s : shapes) s->move(dx, dy);
}
void CompositeShape::scale(double f) {
    Point baseC = getCenter();
    for (auto& s : shapes) {
        Point itemC = s->getCenter();
        s->move((itemC.x - baseC.x) * (f - 1), (itemC.y - baseC.y) * (f - 1));
        s->scale(f);
    }
}
std::string CompositeShape::getName() const { return "COMPOSITE"; }
void CompositeShape::print() const {
    Shape::print(); std::cout << ":\n";
    for (const auto& s : shapes) { std::cout << "  "; s->print(); std::cout << "\n"; }
}