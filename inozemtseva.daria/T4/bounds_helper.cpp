#include "bounds_helper.h"
#include "rectangle.h"
#include "ring.h"
#include "square.h"
#include <algorithm>

std::pair<Point, Point> getBoundingBox(const Shape& shape)
{
  if (const auto* rect = dynamic_cast<const Rectangle*>(&shape))
  {
    return {rect->bottom_left_, rect->top_right_};
  }
  if (const auto* ring = dynamic_cast<const Ring*>(&shape))
  {
    Point c = ring->getCenter();
    double R = ring->outer_radius_;
    return {Point(c.x_ - R, c.y_ - R), Point(c.x_ + R, c.y_ + R)};
  }
  if (const auto* sq = dynamic_cast<const Square*>(&shape))
  {
    Point bl = sq->bottom_left_;
    double s = sq->side_length_;
    return {bl, Point(bl.x_ + s, bl.y_ + s)};
  }
  return {Point(0.0, 0.0), Point(0.0, 0.0)};
}
