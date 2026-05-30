#include "composite_shape.h"
#include <algorithm>
#include <iomanip>
#include <limits>
#include <stdexcept>

void CompositeShape::add(std::unique_ptr<Shape> shape)
{
  if (shape)
  {
    shapes_.push_back(std::move(shape));
  }
}

double CompositeShape::getArea() const
{
  double area = 0.0;
  for (const auto& shape : shapes_)
  {
    area += shape->getArea();
  }
  return area;
}

Point CompositeShape::getCenter() const
{
  if (shapes_.empty())
  {
    return Point(0.0, 0.0);
  }

  auto bounds = getBoundingRect();
  Point bottom_left = bounds.first;
  Point top_right = bounds.second;

  Point center{};
  center.x_ = (bottom_left.x_ + top_right.x_) / 2.0;
  center.y_ = (bottom_left.y_ + top_right.y_) / 2.0;
  return center;
}

void CompositeShape::move(double dx, double dy)
{
  for (auto& shape : shapes_)
  {
    shape->move(dx, dy);
  }
}

void CompositeShape::scale(double coefficient)
{
  if (coefficient <= 0 || shapes_.empty())
  {
     throw std::invalid_argument("Scale coefficient must be positive");
  }
  if (shapes_.empty())
  {
    return;
  }
  Point old_center = getCenter();
  for (auto& shape : shapes_)
  {
    Point shape_center = shape->getCenter();
    double vec_x = shape_center.x_ - old_center.x_;
    double vec_y = shape_center.y_ - old_center.y_;

    shape->scale(coefficient);

    Point new_shape_center = shape->getCenter();
    double dx = old_center.x_ + vec_x * coefficient - new_shape_center.x_;
    double dy = old_center.y_ + vec_y * coefficient - new_shape_center.y_;
    shape->move(dx, dy);
  }
}

const char* CompositeShape::getName() const
{
  return "COMPOSITE";
}

std::pair<Point, Point> CompositeShape::getBoundingRect() const
{
  if (shapes_.empty())
  {
    return {Point(0.0, 0.0), Point(0.0, 0.0)};
  }

  double min_x = std::numeric_limits<double>::max();
  double max_x = std::numeric_limits<double>::lowest();
  double min_y = std::numeric_limits<double>::max();
  double max_y = std::numeric_limits<double>::lowest();

  for (const auto& shape : shapes_)
  {
    auto bounds = shape->getBoundingBox();
    Point bl = bounds.first;
    Point tr = bounds.second;

    min_x = std::min(min_x, bl.x_);
    max_x = std::max(max_x, tr.x_);
    min_y = std::min(min_y, bl.y_);
    max_y = std::max(max_y, tr.y_);
  }

  return {Point(min_x, min_y), Point(max_x, max_y)};
}

void CompositeShape::printContents(std::ostream& os) const
{
  os << std::fixed << std::setprecision(2);
  for (size_t i = 0; i < shapes_.size(); ++i)
  {
    if (i > 0)
    {
      os << ", ";
    }
    Point c = shapes_[i]->getCenter();
    double a = shapes_[i]->getArea();
    os << shapes_[i]->getName()
       << ", (" << c.x_ << ", " << c.y_ << "), "
       << a;
  }
}

std::pair<Point, Point> CompositeShape::getBoundingBox() const
{
    return getBoundingRect();
}