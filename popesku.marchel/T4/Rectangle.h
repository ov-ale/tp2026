#ifndef WORKOUT_RECTANGLE_H
#define WORKOUT_RECTANGLE_H

#include "Shape.h"

class Rectangle : public Shape {
public:
    Rectangle(const Point& bl, const Point& tr);

    double getArea() const override;
    Point getCenter() const override;

    void move(double dx, double dy) override;
    void scale(double factor) override;

    std::string getName() const override;

    void print() const override;

private:
    Point bottomLeft_;
    Point topRight_;

};

#endif //WORKOUT_RECTANGLE_H