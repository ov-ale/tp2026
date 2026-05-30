#include "ring.h"
#include <algorithm>
#include <stdexcept>

constexpr double kPi = 3.141592653589793;

Ring::Ring(const Point& center, double outer_radius, double inner_radius)
    : center_(center),
      outer_radius_(outer_radius),
      inner_radius_(inner_radius)
{
  if (outer_radius <= 0)  
  {
      throw std::invalid_argument("Outer radius must be positive");
  }
  if (inner_radius < 0)  
  {
      throw std::invalid_argument("Inner radius cannot be negative");
  }
  if (inner_radius > outer_radius)  
  {
      throw std::invalid_argument("Inner radius cannot be greater than outer radius");
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
    throw std::invalid_argument("Scale coefficient must be positive");;
  }
  outer_radius_ *= coefficient;
  inner_radius_ *= coefficient;
}

const char* Ring::getName() const
{
  return "RING";
}

std::pair<Point, Point> Ring::getBoundingBox() const
{
    return {Point(center_.x_ - outer_radius_, center_.y_ - outer_radius_),
            Point(center_.x_ + outer_radius_, center_.y_ + outer_radius_)};
}