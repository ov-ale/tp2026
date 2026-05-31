#ifndef RECTANGLE_H
#define RECTANGLE_H
#include "shape.h"
#include <utility>

class Rectangle : public Shape {
  public:
  Rectangle(const Point& bottom_left, const Point& top_right);
  double getArea() const override;
  Point getCenter() const override;
  void move(double dx, double dy) override;
  void scale(double coefficient) override;
  const char* getName() const override;
  std::pair<Point, Point> getBoundingBox() const override;
  private:
  Point bottom_left_;
  Point top_right_;
};

#endif
