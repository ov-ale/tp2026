#include "rectangle.h"
#include <algorithm>

Rectangle::Rectangle(const Point& bottom_left, const Point& top_right)
    : bottom_left_(std::min(bottom_left.x_, top_right.x_),
                   std::min(bottom_left.y_, top_right.y_)),
      top_right_(std::max(bottom_left.x_, top_right.x_),
                 std::max(bottom_left.y_, top_right.y_)) {}

double Rectangle::getArea() const
{
  return (top_right_.x_ - bottom_left_.x_) * (top_right_.y_ - bottom_left_.y_);
}

Point Rectangle::getCenter() const
{
  Point center{};
  center.x_ = (bottom_left_.x_ + top_right_.x_) / 2.0;
  center.y_ = (bottom_left_.y_ + top_right_.y_) / 2.0;
  return center;
}

void Rectangle::move(double dx, double dy)
{
  bottom_left_.x_ += dx;
  bottom_left_.y_ += dy;
  top_right_.x_ += dx;
  top_right_.y_ += dy;
}

void Rectangle::scale(double coefficient)
{
  if (coefficient <= 0)
  {
    return;
  }

  Point center = getCenter();
  double half_width = (top_right_.x_ - bottom_left_.x_) / 2.0 * coefficient;
  double half_height = (top_right_.y_ - bottom_left_.y_) / 2.0 * coefficient;

  bottom_left_.x_ = center.x_ - half_width;
  bottom_left_.y_ = center.y_ - half_height;
  top_right_.x_ = center.x_ + half_width;
  top_right_.y_ = center.y_ + half_height;
}

const char* Rectangle::getName() const
{
  return "RECTANGLE";
}
