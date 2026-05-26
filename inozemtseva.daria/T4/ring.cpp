#include "ring.h"
#include <algorithm>

constexpr double kPi = 3.141592653589793;

Ring::Ring(const Point& center, double outer_radius, double inner_radius)
    : center_(center),
      outer_radius_(std::max(outer_radius, inner_radius)),
      inner_radius_(std::min(outer_radius, inner_radius))
{
  if (outer_radius_ <= 0)
  {
    outer_radius_ = 1.0;
  }
  if (inner_radius_ > outer_radius_)
  {
    inner_radius_ = 0.0;
  }
}

double Ring::getArea() const
{
  return kPi * (outer_radius_ * outer_radius_ - inner_radius_ * inner_radius_);
}

Point Ring::getCenter() const
{
  return center_;
}

void Ring::move(double dx, double dy)
{
  center_.x_ += dx;
  center_.y_ += dy;
}

void Ring::scale(double coefficient)
{
  if (coefficient <= 0)
  {
    return;
  }
  outer_radius_ *= coefficient;
  inner_radius_ *= coefficient;
}

const char* Ring::getName() const
{
  return "RING";
}
