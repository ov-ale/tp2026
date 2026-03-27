#include <iostream>
#include <string>
#include "Shape.h"
#include "Rectangle.h"
#include "Point.h"

#ifndef ISOSCELESTRAPEZOID_H
#define ISOSCELESTRAPEZOID_H

class IsoscelesTrapezoid : public Shape {
public:
    IsoscelesTrapezoid() = default;
    IsoscelesTrapezoid(Point p, double a, double b, double c) :
        bottomLeft_(p), bottomBase_(a),
        topBase_(b), height_(c)
    { }

    double getArea() const override;
    Point getCenter() const override;
    void move(double x, double y) override;
    void scale(double factor) override;
    std::string getName() const override;

    // help func
    Point getTopRight() const override {
        Point p = bottomLeft_;
        return Point(p.x_ + bottomBase_, p.y_ + height_);
    }
    Point getBottomLeft() const override {
        return bottomLeft_;
    }
private:
    Point bottomLeft_;
    double bottomBase_;
    double topBase_;
    double height_;
};

#endif // !ISOSCELESTRAPEZOID_H