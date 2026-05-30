#ifndef COMPOSITE_SHAPE_H
#define COMPOSITE_SHAPE_H
#include "shape.h"
#include <memory>
#include <ostream>
#include <vector>

class CompositeShape : public Shape {
 public:
  CompositeShape() = default;
  void add(std::unique_ptr<Shape> shape);
  double getArea() const override;
  Point getCenter() const override;
  void move(double dx, double dy) override;
  void scale(double coefficient) override;
  const char* getName() const override;
  void printContents(std::ostream& os) const;
  std::pair<Point, Point> getBoundingBox() const override;
 private:
  std::vector<std::unique_ptr<Shape>> shapes_;
  std::pair<Point, Point> getBoundingRect() const;
};

#endif
