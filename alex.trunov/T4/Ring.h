#include <iostream>
#include <string>
#include "Point.h"
#include "Shape.h"
#include "Rectangle.h"

#ifndef RING_H
#define RING_H

class Ring : public Shape {
public:
    Ring() = default;
    Ring(double r1, double r2, Point p) :
        minRad_(r1), maxRad_(r2), center_(p)
    { }

    double getArea() const override;
    Point getCenter() const override;
    void move(double x, double y) override;
    void scale(double factor) override;
    std::string getName() const override;

    // help func
    Point getTopRight() const override {
        Point p = getCenter();
        double rad = maxRad_;
        return Point(p.x_ + rad, p.y_ + rad);
    }
    Point getBottomLeft() const override {
        Point p = getCenter();
        double rad = maxRad_;
        return Point(p.x_ - rad, p.y_ - rad);
    }
private:
    double minRad_;
    double maxRad_;
    Point center_;
};
#endif // !RING_H
