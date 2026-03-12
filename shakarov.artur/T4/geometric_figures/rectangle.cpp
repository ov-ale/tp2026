#include "rectangle.h"
#include <iostream>

Rectangle::Rectangle(double x1, double y1, double x2, double y2) {
    this->x1 = x1;
    this->y1 = y1;
    this->x2 = x2;
    this->y2 = y2;
}

double Rectangle::getArea() {
    double width = x2 - x1;
    double height = y2 - y1;
    return width * height;
}

Point Rectangle::getCenter() {
    return Point((x1 + x2) / 2, (y1 + y2) / 2);
}

void Rectangle::move(double dx, double dy) {
    x1 += dx;
    y1 += dy;
    x2 += dx;
    y2 += dy;
}

void Rectangle::scale(double factor) {
    Point center = getCenter();
    // Относительно центра меняем размер
    double newWidth = (x2 - x1) * factor;
    double newHeight = (y2 - y1) * factor;

    x1 = center.x - newWidth / 2;
    x2 = center.x + newWidth / 2;
    y1 = center.y - newHeight / 2;
    y2 = center.y + newHeight / 2;
}

std::string Rectangle::getName() {
    return "RECTANGLE";
}