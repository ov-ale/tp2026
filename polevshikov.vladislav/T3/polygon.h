#ifndef POLYGON_H
#define POLYGON_H

#include <iostream>
#include <vector>

struct Point {
    int x, y;
};

struct Polygon {
    std::vector<Point> points;
};

struct HasNVertices {
    size_t n;
    bool operator()(const Polygon& p) const { return p.points.size() == n; }
};

std::istream& operator>>(std::istream& in, Polygon& dest);

double getArea(const Polygon& poly);
bool hasRightAngle(const Polygon& poly);

bool compareByArea(const Polygon& a, const Polygon& b);
bool compareBySize(const Polygon& a, const Polygon& b);
bool isEven(const Polygon& p);
bool isOdd(const Polygon& p);
double sumArea(double sum, const Polygon& p);

bool cmpByX(const Point& a, const Point& b);
bool cmpByY(const Point& a, const Point& b);

int getMinX(int cur, const Polygon& p);
int getMaxX(int cur, const Polygon& p);
int getMinY(int cur, const Polygon& p);
int getMaxY(int cur, const Polygon& p);

#endif
