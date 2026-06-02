#include "polygon.h"
#include <iostream>
#include <string>
#include <limits>
#include <cmath>
#include <numeric>
#include <algorithm>
#include <iterator>

struct DelimiterI0 {
    char expected;
};

std::istream& operator>>(std::istream& in, DelimiterI0&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) {
        return in;
    }

    char c = ' ';
    in >> c;

    if (in && (c != dest.expected)) {
        in.setstate(std::ios::failbit);
    }

    return in;
}

std::istream& operator>>(std::istream& in, Point& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) {
        return in;
    }
    return in >> DelimiterI0{'('} >> dest.x >> DelimiterI0{';'} >> dest.y >> DelimiterI0{')'};
}

std::istream& operator>>(std::istream& in, Polygon& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) {
        return in;
    }

    size_t cnt = 0;
    if (!(in >> cnt) || cnt < 3) {
        in.setstate(std::ios::failbit);
        return in;
    }

    std::vector<Point> tmp_points(cnt);
    std::copy_n(std::istream_iterator<Point>(in), cnt, tmp_points.begin());
    if (!in) return in;
    dest.points = std::move(tmp_points);
    return in;
}

struct CrossProductAccum {
    const Polygon& poly;
    double operator()(double sum, size_t i) const {
        const Point& a = poly.points[i];
        const Point& b = poly.points[(i + 1) % poly.points.size()];
        return sum + (static_cast<double>(a.x) * b.y - static_cast<double>(a.y) * b.x);
    }
};

double getArea(const Polygon& poly) {
    size_t n = poly.points.size();
    std::vector<size_t> idx(n);
    std::iota(idx.begin(), idx.end(), 0);
    double area = std::accumulate(idx.begin(), idx.end(), 0.0, CrossProductAccum{poly});
    return std::abs(area) / 2.0;
}

struct rightAngle {
    const Polygon& poly;
    bool operator()(size_t i) const {
        size_t n = poly.points.size();
        const Point& a = poly.points[i];
        const Point& b = poly.points[(i + 1) % n];
        const Point& c = poly.points[(i + 2) % n];
        long long v1x = b.x - a.x, v1y = b.y - a.y;
        long long v2x = c.x - b.x, v2y = c.y - b.y;
        return (v1x * v2x + v1y * v2y) == 0;
    }
};

bool hasRightAngle(const Polygon& poly) {
    size_t n = poly.points.size();
    std::vector<size_t> idx(n);
    std::iota(idx.begin(), idx.end(), 0);
    return std::any_of(idx.begin(), idx.end(), rightAngle{poly});
}

bool isEven(const Polygon& p) {
    return p.points.size() % 2 == 0;
}

bool isOdd(const Polygon& p) {
    return p.points.size() % 2 != 0;
}

bool compareByArea(const Polygon& a, const Polygon& b) {
    return getArea(a) < getArea(b);
}

bool compareBySize(const Polygon& a, const Polygon& b) {
    return a.points.size() < b.points.size();
}

double sumArea(double sum, const Polygon& p) {
    return sum + getArea(p);
}

bool cmpByX(const Point& a, const Point& b) {
    return a.x < b.x;
}
bool cmpByY(const Point& a, const Point& b) {
    return a.y < b.y;
}

int getMinX(int cur, const Polygon& p) {
    return std::min(cur, std::min_element(p.points.begin(), p.points.end(), cmpByX)->x);
}
int getMaxX(int cur, const Polygon& p) {
    return std::max(cur, std::max_element(p.points.begin(), p.points.end(), cmpByX)->x);
}
int getMinY(int cur, const Polygon& p) {
    return std::min(cur, std::min_element(p.points.begin(), p.points.end(), cmpByY)->y);
}
int getMaxY(int cur, const Polygon& p) {
    return std::max(cur, std::max_element(p.points.begin(), p.points.end(), cmpByY)->y);
}
