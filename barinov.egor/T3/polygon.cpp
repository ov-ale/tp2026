#include "polygon.hpp"
#include <cmath>
#include <limits>
#include <algorithm>
#include <sstream>
#include <string>

bool operator==(const Point& a, const Point& b)
{
  return a.x == b.x && a.y == b.y;
}

bool operator!=(const Point& a, const Point& b)
{
  return !(a == b);
}

bool operator==(const Polygon& a, const Polygon& b)
{
  if (a.points.size() != b.points.size())
  {
    return false;
  }
  for (size_t i = 0; i < a.points.size(); ++i)
  {
    if (a.points[i] != b.points[i])
    {
      return false;
    }
  }
  return true;
}

bool operator!=(const Polygon& a, const Polygon& b)
{
  return !(a == b);
}

std::istream& operator>>(std::istream& in, Point& point)
{
  char c = '\0';
  in >> c;
  if (c != '(')
  {
    in.setstate(std::ios::failbit);
    return in;
  }
  in >> point.x;
  in >> c;
  if (c != ';')
  {
    in.setstate(std::ios::failbit);
    return in;
  }
  in >> point.y;
  in >> c;
  if (c != ')')
  {
    in.setstate(std::ios::failbit);
    return in;
  }
  return in;
}

std::istream& operator>>(std::istream& in, Polygon& polygon)
{
  polygon.points.clear();
  size_t vertexCount = 0;
  in >> vertexCount;
  if (!in || vertexCount < 3)
  {
    in.setstate(std::ios::failbit);
    return in;
  }
  for (size_t i = 0; i < vertexCount; ++i)
  {
    Point p;
    in >> p;
    if (!in)
    {
      in.setstate(std::ios::failbit);
      return in;
    }
    polygon.points.push_back(p);
  }
  return in;
}

double getArea(const Polygon& polygon)
{
  double area = 0.0;
  size_t n = polygon.points.size();
  for (size_t i = 0; i < n; ++i)
  {
    const Point& p1 = polygon.points[i];
    const Point& p2 = polygon.points[(i + 1) % n];
    area += static_cast<double>(p1.x * p2.y - p2.x * p1.y);
  }
  return std::fabs(area) / 2.0;
}

void getBoundingBox(const std::vector<Polygon>& polygons,
                    Point& bottomLeft,
                    Point& topRight)
{
  if (polygons.empty())
  {
    bottomLeft = Point{0, 0};
    topRight = Point{0, 0};
    return;
  }
  int minX = std::numeric_limits<int>::max();
  int minY = std::numeric_limits<int>::max();
  int maxX = std::numeric_limits<int>::lowest();
  int maxY = std::numeric_limits<int>::lowest();
  for (const auto& polygon : polygons)
  {
    for (const auto& point : polygon.points)
    {
      if (point.x < minX) minX = point.x;
      if (point.y < minY) minY = point.y;
      if (point.x > maxX) maxX = point.x;
      if (point.y > maxY) maxY = point.y;
    }
  }
  bottomLeft = Point{minX, minY};
  topRight = Point{maxX, maxY};
}

bool isPointInBoundingBox(const Point& point,
                          const Point& bottomLeft,
                          const Point& topRight)
{
  return point.x >= bottomLeft.x && point.x <= topRight.x &&
         point.y >= bottomLeft.y && point.y <= topRight.y;
}

bool isPolygonInBoundingBox(const Polygon& polygon,
                            const Point& bottomLeft,
                            const Point& topRight)
{
  for (const auto& point : polygon.points)
  {
    if (!isPointInBoundingBox(point, bottomLeft, topRight))
    {
      return false;
    }
  }
  return true;
}