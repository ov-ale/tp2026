#include "Geom.h"
#include <algorithm>
#include <numeric>
#include <cmath>
namespace {
    struct CrossProduct {
        long long operator()(const Point& a, const Point& b) const {
            return static_cast<long long>(a.x)*b.y-static_cast<long long>(b.x)*a.y;
        }
    };
    struct IsRightAngle {
        bool operator()(const Point& prev, const Point& curr, const Point& next) const {
            long long dot=static_cast<long long>(curr.x-prev.x)*(next.x-curr.x)
                         +static_cast<long long>(curr.y-prev.y)*(next.y-curr.y);
            return dot==0;
        }
    };
    struct ReadPoint {
        std::istream& in;
        explicit ReadPoint(std::istream& s):in(s) {}
        Point operator()() const {
            Point p{0, 0};
            char open, sep, close;
            if (!(in>>open>>p.x>>sep>>p.y>>close) || open!='(' || sep!=';' || close!=')') {
                in.setstate(std::ios::failbit);
            }
            return p;
        }
    };
}
bool Point::operator==(const Point& other) const {
    return x==other.x && y==other.y;
}
bool Polygon::operator==(const Polygon& other) const {
    return points.size()==other.points.size() && std::equal(points.begin(),
           points.end(), other.points.begin());
}
std::istream& operator>>(std::istream& in, Polygon& p) {
    size_t n=0;
    if (!(in>>n) || n<3) {
        if (in) {
            in.setstate(std::ios::failbit);
        }
        return in;
    }
    p.points.resize(n);
    std::generate(p.points.begin(), p.points.end(), ReadPoint(in));
    if (!in) {
        p.points.clear();
    }
    return in;
}
double getPolygonArea(const Polygon& p) {
    const auto& pts=p.points;
    if (pts.size()<3) {
        return 0.0;
    }
    long long area=std::inner_product(pts.begin(), std::prev(pts.end()),
        std::next(pts.begin()), 0LL, std::plus<long long>(), CrossProduct());
    area+=CrossProduct()(pts.back(), pts.front());
    return std::abs(static_cast<double>(area))/2.0;
}
bool isRectangle(const Polygon& p) {
    if (p.points.size()!=4) {
        return false;
    }
    const auto& pts=p.points;
    IsRightAngle right;
    return right(pts[3], pts[0], pts[1]) && right(pts[0], pts[1], pts[2])
        && right(pts[1], pts[2], pts[3]) && right(pts[2], pts[3], pts[0]);
}
