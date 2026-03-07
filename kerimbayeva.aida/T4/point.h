#pragma once

#include <cmath>
#include <iomanip>
#include <ostream>

struct Point {
    double x{ 0.0 };
    double y{ 0.0 };

    Point() = default;
    Point(double x_, double y_) : x(x_), y(y_) {}

    Point operator+(const Point& other) const { 
        return { x + other.x, y + other.y };
    }
    Point operator-(const Point& other) const {
        return { x - other.x, y - other.y }; 
    }
    Point operator*(double k) const {
        return { x * k, y * k };
    }
    Point operator/(double k) const { return 
        { x / k, y / k };
    }

    double distance_to(const Point& other) const {
        double dx = x - other.x;
        double dy = y - other.y;
        return std::hypot(dx, dy);
    }
};

inline std::ostream& operator<<(std::ostream& os, const Point& p) {
    os << std::fixed << std::setprecision(2) << "(" << p.x << ", " << p.y << ")";
    return os;
}