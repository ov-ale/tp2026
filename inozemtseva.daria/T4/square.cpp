#include "square.h"

Square::Square(const Point& bottom_left, double side_length)
    : bottom_left_(bottom_left),
      side_length_(side_length > 0 ? side_length : 1.0) {}

double Square::getArea() const
{
  return side_length_ * side_length_;
}

Point Square::getCenter() const
{
  Point center{};
  center.x_ = bottom_left_.x_ + side_length_ / 2.0;
  center.y_ = bottom_left_.y_ + side_length_ / 2.0;
  return center;
}

void Square::move(double dx, double dy)
{
  bottom_left_.x_ += dx;
  bottom_left_.y_ += dy;
}

void Square::scale(double coefficient)
{
  if (coefficient <= 0)
  {
    return;
  }

  Point old_center = getCenter();
  side_length_ *= coefficient;
  Point new_center = getCenter();
  move(old_center.x_ - new_center.x_, old_center.y_ - new_center.y_);
}

const char* Square::getName() const
{
  return "SQUARE";
}
