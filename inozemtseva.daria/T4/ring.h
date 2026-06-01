#ifndef RING_H
#define RING_H
#include "shape.h"
#include <utility>

class Ring : public Shape {
  public:
  Ring(const Point& center, double outer_radius, double inner_radius);
  double getArea() const override;
  Point getCenter() const override;
  void move(double dx, double dy) override;
  void scale(double coefficient) override;
  const char* getName() const override;
  std::pair<Point, Point> getBoundingBox() const override;
  private:
  Point center_;
  double outer_radius_;
  double inner_radius_;
};

#endif
