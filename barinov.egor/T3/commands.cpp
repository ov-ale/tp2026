#include "commands.hpp"
#include <algorithm>
#include <limits>
#include <cmath>

bool isEven(int n)
{
  return n % 2 == 0;
}

bool isOdd(int n)
{
  return n % 2 != 0;
}

double getAreaEven(const std::vector<Polygon>& polygons)
{
  double sum = 0.0;
  for (const auto& polygon : polygons)
  {
    if (isEven(polygon.points.size()))
    {
      sum += getArea(polygon);
    }
  }
  return sum;
}

double getAreaOdd(const std::vector<Polygon>& polygons)
{
  double sum = 0.0;
  for (const auto& polygon : polygons)
  {
    if (isOdd(polygon.points.size()))
    {
      sum += getArea(polygon);
    }
  }
  return sum;
}

double getAreaMean(const std::vector<Polygon>& polygons)
{
  if (polygons.empty())
  {
    return 0.0;
  }
  double sum = 0.0;
  for (const auto& polygon : polygons)
  {
    sum += getArea(polygon);
  }
  return sum / static_cast<double>(polygons.size());
}

double getAreaByVertexCount(const std::vector<Polygon>& polygons, size_t vertexCount)
{
  double sum = 0.0;
  for (const auto& polygon : polygons)
  {
    if (polygon.points.size() == vertexCount)
    {
      sum += getArea(polygon);
    }
  }
  return sum;
}

double getMaxArea(const std::vector<Polygon>& polygons)
{
  if (polygons.empty())
  {
    return 0.0;
  }
  double maxArea = getArea(polygons[0]);
  for (const auto& polygon : polygons)
  {
    double area = getArea(polygon);
    if (area > maxArea)
    {
      maxArea = area;
    }
  }
  return maxArea;
}

size_t getMaxVertexes(const std::vector<Polygon>& polygons)
{
  if (polygons.empty())
  {
    return 0;
  }
  size_t maxVertexes = polygons[0].points.size();
  for (const auto& polygon : polygons)
  {
    if (polygon.points.size() > maxVertexes)
    {
      maxVertexes = polygon.points.size();
    }
  }
  return maxVertexes;
}

double getMinArea(const std::vector<Polygon>& polygons)
{
  if (polygons.empty())
  {
    return 0.0;
  }
  double minArea = getArea(polygons[0]);
  for (const auto& polygon : polygons)
  {
    double area = getArea(polygon);
    if (area < minArea)
    {
      minArea = area;
    }
  }
  return minArea;
}

size_t getMinVertexes(const std::vector<Polygon>& polygons)
{
  if (polygons.empty())
  {
    return 0;
  }
  size_t minVertexes = polygons[0].points.size();
  for (const auto& polygon : polygons)
  {
    if (polygon.points.size() < minVertexes)
    {
      minVertexes = polygon.points.size();
    }
  }
  return minVertexes;
}

size_t countEven(const std::vector<Polygon>& polygons)
{
  size_t count = 0;
  for (const auto& polygon : polygons)
  {
    if (isEven(polygon.points.size()))
    {
      ++count;
    }
  }
  return count;
}

size_t countOdd(const std::vector<Polygon>& polygons)
{
  size_t count = 0;
  for (const auto& polygon : polygons)
  {
    if (isOdd(polygon.points.size()))
    {
      ++count;
    }
  }
  return count;
}

size_t countByVertexCount(const std::vector<Polygon>& polygons, size_t vertexCount)
{
  size_t count = 0;
  for (const auto& polygon : polygons)
  {
    if (polygon.points.size() == vertexCount)
    {
      ++count;
    }
  }
  return count;
}

size_t echoCommand(std::vector<Polygon>& polygons, const Polygon& target)
{
  size_t added = 0;
  for (size_t i = 0; i < polygons.size(); ++i)
  {
    if (polygons[i] == target)
    {
      polygons.insert(polygons.begin() + i + 1, target);
      ++added;
      ++i;
    }
  }
  return added;
}

bool inframeCommand(const std::vector<Polygon>& polygons, const Polygon& target)
{
  if (polygons.empty())
  {
    return false;
  }
  Point bottomLeft, topRight;
  getBoundingBox(polygons, bottomLeft, topRight);
  return isPolygonInBoundingBox(target, bottomLeft, topRight);
}
