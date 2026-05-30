#ifndef SQUARE_H
#define SQUARE_H
#include "shape.h"
#include <utility>

class Square : public Shape {
public:
  Square(const Point& bottom_left, double side_length);
  double getArea() const override;
  Point getCenter() const override;
  void move(double dx, double dy) override;
  void scale(double coefficient) override;
  const char* getName() const override;
  std::pair<Point, Point> getBoundingBox() const override;
 private:
  Point bottom_left_;
  double side_length_;
};

#endif
