#ifndef RHOMBUS_H
#define RHOMBUS_H

#include "shape.h"

class Rhombus : public Shape {
private:
    Point center;
    double diagX;  // диагональ по горизонтальной оси
    double diagY;  // диагональ по вертикальной оси
    
public:
    Rhombus(const Point& center, double diagX, double diagY);
    
    double getArea() const override;
    Point getCenter() const override;
    void move(double dx, double dy) override;
    void scale(double factor) override;
    std::string getName() const override;
};

#endif 