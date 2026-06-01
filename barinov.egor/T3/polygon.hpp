#ifndef POLYGON_HPP
#define POLYGON_HPP

#include <vector>
#include <iostream>

struct Point
{
  int x, y;
};

struct Polygon
{
  std::vector<Point> points;
};

bool operator==(const Point& a, const Point& b);
bool operator!=(const Point& a, const Point& b);

bool operator==(const Polygon& a, const Polygon& b);
bool operator!=(const Polygon& a, const Polygon& b);

std::istream& operator>>(std::istream& in, Point& point);
std::istream& operator>>(std::istream& in, Polygon& polygon);

double getArea(const Polygon& polygon);

void getBoundingBox(
  const std::vector<Polygon>& polygons,
  Point& bottomLeft,
  Point& topRight
);

bool isPointInBoundingBox(const Point& point,
                          const Point& bottomLeft,
                          const Point& topRight);

bool isPolygonInBoundingBox(const Polygon& polygon,
                            const Point& bottomLeft,
                            const Point& topRight);

#endif