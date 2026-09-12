#ifndef COMPOSITESHAPE_H
#define COMPOSITESHAPE_H

#include <memory>
#include <vector>
#include "shape.h"

// Составная фигура из произвольного набора простых фигур.
// Владеет составляющими фигурами (unique_ptr) - их время жизни совпадает
// со временем жизни составной фигуры.
//
// Примечание про getCenter(): т.к. интерфейс Shape не предоставляет
// доступа к геометрической "протяжённости" произвольной фигуры (радиусу,
// сторонам и т.п.), центр ограничивающего прямоугольника здесь вычисляется
// по центрам составляющих фигур (минимум/максимум их координат). Для
// площади и перемещения (move) это ограничение не имеет значения - они
// считаются точно.
class CompositeShape : public Shape {
public:
    CompositeShape() = default;

    void addShape(std::unique_ptr<Shape> shape);

    double getArea() const override;
    Point getCenter() const override;
    void move(double dx, double dy) override;
    void scale(double factor) override;
    std::string getName() const override;

    size_t getCount() const;
    const Shape& getShapeAt(size_t index) const;

private:
    std::vector<std::unique_ptr<Shape>> shapes_;
};

#endif
