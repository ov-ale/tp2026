#ifndef POINT_H
#define POINT_H

struct Point {
  double x_;
  double y_;

  Point(double x_val = 0.0, double y_val = 0.0) : x_(x_val), y_(y_val) {}
};

#endif
