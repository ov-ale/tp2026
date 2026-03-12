#ifndef COMPOSITESHAPE_H
#define COMPOSITESHAPE_H

#include "shape.h"

class CompositeShape : public Shape {
private:
    Shape** shapes;     // массив указателей на фигуры
    int count;          // количество фигур
    int capacity;       // вместимость массива

public:
    CompositeShape();
    ~CompositeShape();

    void addShape(Shape* shape);
    double getArea() override;
    Point getCenter() override;
    void move(double dx, double dy) override;
    void scale(double factor) override;
    std::string getName() override;

    Shape** getShapes() { return shapes; }
    int getCount() { return count; }
};

#endif