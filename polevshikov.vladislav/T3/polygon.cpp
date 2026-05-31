#include "polygon.h"
#include <iostream>
#include <string>
#include <limits>
#include <cmath>
#include <numeric>

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

    std::vector<Point> tmp_points;
    for (size_t i = 0; i < cnt; ++i) {
        Point p;

        if (in >> p) {
            tmp_points.push_back(p);
        } else {
            return in;
        }
    }
    dest.points = std::move(tmp_points);
    return in;
}

double getArea(const Polygon& poly) {
    size_t n = poly.points.size();
    std::vector<size_t> idx(n);

    std::iota(idx.begin(), idx.end(), 0);
    long long area = std::accumulate(idx.begin(), idx.end(), 0.0, [&](double sum, size_t i) {
        const Point& a = poly.points[i];
        const Point& b = poly.points[(i + 1) % n];
        return sum + ((long long)a.x * b.y - (long long)a.y * b.x);
    });
    return std::abs(double(area)) / 2.0;
}

bool hasRightAngle(const Polygon& poly) {
    size_t n = poly.points.size();
    std::vector<size_t> idx(n);
    std::iota(idx.begin(), idx.end(), 0);
    return std::any_of(idx.begin(), idx.end(), [&](size_t i) {
        const Point& a = poly.points[i];
        const Point& b = poly.points[(i + 1) % n];
        const Point& c = poly.points[(i + 2) % n];
        long long v1x = b.x - a.x, v1y = b.y - a.y;
        long long v2x = c.x - b.x, v2y = c.y - b.y;
        return (v1x * v2x + v1y * v2y) == 0;
    });
}
