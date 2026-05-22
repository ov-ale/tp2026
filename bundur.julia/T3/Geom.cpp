#include <algorithm>
#include <iterator>
#include <numeric>
#include <cmath>
#include "Geom.h"
std::istream& operator>>(std::istream& is, Point& pt) {
    char p1, p2, sep;
    if (is>>p1>>pt.x>>sep>>pt.y>>p2) {
        if (p1!='(' || sep!=';' || p2!=')') {
            is.setstate(std::ios::failbit);
        }
    }
    return is;
}
std::istream& operator>>(std::istream& is, Polygon& poly) {
    size_t size;
    if (is>>size) {
        if (size<3) {
            is.setstate(std::ios::failbit);
            return is;
        }
        std::vector<Point> temp;
        std::copy_n(std::istream_iterator<Point>(is), size, std::back_inserter(temp));
        if (is) {
            poly.points=std::move(temp);
        }
    }
    return is;
}
bool operator==(const Point& lhs, const Point& rhs) {
    return lhs.x==rhs.x && lhs.y==rhs.y;
}

bool operator==(const Polygon& lhs, const Polygon& rhs) {
    if (lhs.points.size()!=rhs.points.size()) {
      return false;
    }
    return std::equal(lhs.points.begin(), lhs.points.end(), rhs.points.begin());
}
double getPolygonArea(const Polygon& poly) {
    if (poly.points.size()<3) {
      return 0.0;
    }
    auto cross_product=[](double sum, const Point& p1, const Point& p2) {
        return sum+(p1.x*p2.y-p1.y*p2.x);
    };
    double area=std::inner_product(poly.points.begin(), poly.points.end()-1,
        poly.points.begin() + 1,
        0.0,
        std::plus<>(),
        [](const Point& p1, const Point& p2) {
        return (p1.x*p2.y-p1.y*p2.x);
        }
    );
    area+=cross_product(0.0, poly.points.back(), poly.points.front());
    return std::abs(area)/2.0;
}
bool isRectangle(const Polygon& poly) {
    if (poly.points.size()!=4) {
      return false;
    }
    auto getSqDist=[](const Point& a, const Point& b) {
        return (a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y);
    };
    int d2_12=getSqDist(poly.points[0], poly.points[1]);
    int d2_23=getSqDist(poly.points[1], poly.points[2]);
    int d2_34=getSqDist(poly.points[2], poly.points[3]);
    int d2_41=getSqDist(poly.points[3], poly.points[0]);
    int d2_13=getSqDist(poly.points[0], poly.points[2]);
    int d2_24=getSqDist(poly.points[1], poly.points[3]);
    return d2_12>0 && d2_12==d2_34 && d2_23==d2_41 && d2_13==d2_24;
}
