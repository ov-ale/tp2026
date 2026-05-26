#ifndef BOUNDS_HELPER_H
#define BOUNDS_HELPER_H
#include "shape.h"
#include "point.h"
#include <utility>

std::pair<Point, Point> getBoundingBox(const Shape& shape);

#endif
