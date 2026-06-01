#ifndef SHAPE_H
#define SHAPE_H
#include "point.h"
#include <utility>

class Shape {
  public:
  virtual ~Shape() = default;
  virtual double getArea() const = 0;
  virtual Point getCenter() const = 0;
  virtual void move(double dx, double dy) = 0;
  virtual void scale(double coefficient) = 0;
  virtual const char* getName() const = 0;
  virtual std::pair<Point, Point> getBoundingBox() const = 0;
};

#endif
