#ifndef RING_H
#define RING_H

#include "shape.h"

class Ring : public Shape {
private:
    Point center_;
    double outerRadius_;
    double innerRadius_;
public:
    Ring(const Point& center, double outer, double inner);
    double getArea() const override;
    Point getCenter() const override;
    void move(double dx, double dy) override;
    void scale(double factor) override;
    std::string getName() const override;
    double getMinX() const override;
    double getMinY() const override;
    double getMaxX() const override;
    double getMaxY() const override;
};

#endif
