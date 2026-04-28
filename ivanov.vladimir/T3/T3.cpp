#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <limits>
#include <cmath>
#include <stdexcept>
#include <iterator>
#include <functional>

struct Point {
    int x;
    int y;
};

bool operator==(const Point& a, const Point& b) {
    return a.x == b.x && a.y == b.y;
}

struct Polygon {
    std::vector<Point> points;

    Polygon() = default;
    Polygon(const Polygon& other) = default;
    Polygon(Polygon&& other) noexcept = default;
    Polygon& operator=(const Polygon& other) = default;
    Polygon& operator=(Polygon&& other) noexcept = default;
    ~Polygon() = default;
};

bool operator==(const Polygon& a, const Polygon& b) {
    return a.points.size() == b.points.size() &&
           std::equal(a.points.begin(), a.points.end(), b.points.begin());
}

std::istream& operator>>(std::istream& in, Point& pt) {
    std::istream::sentry sentry(in);
    if (!sentry) return in;
    char c1 = 0, c2 = 0, c3 = 0;
    int x = 0, y = 0;
    in >> c1 >> x >> c2 >> y >> c3;
    if (in && c1 == '(' && c2 == ';' && c3 == ')') {
        pt.x = x;
        pt.y = y;
    } else {
        in.setstate(std::ios::failbit);
    }
    return in;
}

std::istream& operator>>(std::istream& in, Polygon& p) {
    std::istream::sentry sentry(in);
    if (!sentry) return in;
    size_t count = 0;
    if (!(in >> count) || count < 3) {
        in.setstate(std::ios::failbit);
        return in;
    }
    std::vector<Point> pts;
    pts.reserve(count);
    std::copy_n(std::istream_iterator<Point>(in), count, std::back_inserter(pts));
    if (in && pts.size() == count) {
        p.points = std::move(pts);
    } else {
        in.setstate(std::ios::failbit);
    }
    return in;
}

struct AreaAcc {
    Point first;
    Point prev;
    double sum;
};

AreaAcc accumulateArea(AreaAcc acc, const Point& curr) {
    acc.sum += static_cast<double>(acc.prev.x) * curr.y - static_cast<double>(curr.x) * acc.prev.y;
    acc.prev = curr;
    return acc;
}

double getArea(const Polygon& p) {
    if (p.points.size() < 3) return 0.0;
    AreaAcc init{p.points.front(), p.points.front(), 0.0};
    AreaAcc res = std::accumulate(std::next(p.points.begin()), p.points.end(), init, accumulateArea);
    res.sum += static_cast<double>(res.prev.x) * res.first.y - static_cast<double>(res.first.x) * res.prev.y;
    return std::abs(res.sum) / 2.0;
}

int ccw(Point a, Point b, Point c) {
    long long val = 1LL * (b.x - a.x) * (c.y - a.y) - 1LL * (b.y - a.y) * (c.x - a.x);
    if (val > 0) return 1;
    if (val < 0) return -1;
    return 0;
}

bool onSegment(Point p, Point a, Point b) {
    return p.x >= std::min(a.x, b.x) && p.x <= std::max(a.x, b.x) &&
           p.y >= std::min(a.y, b.y) && p.y <= std::max(a.y, b.y);
}

bool intersectLines(Point a, Point b, Point c, Point d) {
    int o1 = ccw(a, b, c);
    int o2 = ccw(a, b, d);
    int o3 = ccw(c, d, a);
    int o4 = ccw(c, d, b);
    if (o1 != o2 && o3 != o4) return true;
    if (o1 == 0 && onSegment(c, a, b)) return true;
    if (o2 == 0 && onSegment(d, a, b)) return true;
    if (o3 == 0 && onSegment(a, c, d)) return true;
    if (o4 == 0 && onSegment(b, c, d)) return true;
    return false;
}

struct RayIntersectChecker {
    Point p;
    const Polygon* poly;
    bool operator()(size_t i) const {
        size_t j = (i == 0) ? poly->points.size() - 1 : i - 1;
        Point p1 = poly->points[i];
        Point p2 = poly->points[j];
        if ((p1.y > p.y) != (p2.y > p.y)) {
            double xInt = static_cast<double>(p2.x - p1.x) * (p.y - p1.y) / (p2.y - p1.y) + p1.x;
            return p.x < xInt;
        }
        return false;
    }
};

bool isPointInside(const Polygon& poly, const Point& p) {
    std::vector<size_t> idx(poly.points.size());
    std::iota(idx.begin(), idx.end(), 0);
    size_t count = std::count_if(idx.begin(), idx.end(), RayIntersectChecker{p, &poly});
    return count % 2 != 0;
}
struct EdgeChecker {
    Point p1, p2;
    const Polygon* b;
    bool operator()(size_t idxB) const {
        Point p3 = b->points[idxB];
        Point p4 = b->points[(idxB + 1) % b->points.size()];
        return intersectLines(p1, p2, p3, p4);
    }
};

struct PolyChecker {
    const Polygon* a;
    const Polygon* b;
    std::vector<size_t> idxB;
    PolyChecker(const Polygon* polyA, const Polygon* polyB) : a(polyA), b(polyB) {
        idxB.resize(b->points.size());
        std::iota(idxB.begin(), idxB.end(), 0);
    }
    bool operator()(size_t idxA) const {
        Point p1 = a->points[idxA];
        Point p2 = a->points[(idxA + 1) % a->points.size()];
        return std::any_of(idxB.begin(), idxB.end(), EdgeChecker{p1, p2, b});
    }
};

bool intersectPolygons(const Polygon& a, const Polygon& b) {
    std::vector<size_t> idxA(a.points.size());
    std::iota(idxA.begin(), idxA.end(), 0);
    bool edges = std::any_of(idxA.begin(), idxA.end(), PolyChecker{&a, &b});
    if (edges) return true;
    if (isPointInside(b, a.points[0])) return true;
    if (isPointInside(a, b.points[0])) return true;
    return false;
}

bool isEvenVertices(const Polygon& p) { return p.points.size() % 2 == 0; }
bool isOddVertices(const Polygon& p) { return p.points.size() % 2 != 0; }
bool hasVertices(const Polygon& p, size_t n) { return std::equal_to<size_t>{}(p.points.size(), n); }

double addIfEven(double acc, const Polygon& p) { return isEvenVertices(p) ? acc + getArea(p) : acc; }
double addIfOdd(double acc, const Polygon& p) { return isOddVertices(p) ? acc + getArea(p) : acc; }
double addIfSize(double acc, const Polygon& p, size_t n) { return hasVertices(p, n) ? acc + getArea(p) : acc; }
double addArea(double acc, const Polygon& p) { return acc + getArea(p); }

bool cmpArea(const Polygon& a, const Polygon& b) { return std::less<double>{}(getArea(a), getArea(b)); }
bool cmpVertices(const Polygon& a, const Polygon& b) { return std::less<size_t>{}(a.points.size(), b.points.size()); }

bool isTargetPair(const Polygon& a, const Polygon& b, const Polygon& target) {
    return a == target && b == target;
}

bool checkIntersect(const Polygon& p, const Polygon& target) {
    return intersectPolygons(p, target);
}

size_t parseStrictSizeT(const std::string& str) {
    size_t pos = 0;
    size_t val = std::stoull(str, &pos);
    if (pos != str.length()) throw std::invalid_argument("");
    return val;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Missing filename\n";
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file) {
        std::cerr << "Cannot open file\n";
        return 1;
    }

    std::vector<Polygon> polygons;
    std::copy(std::istream_iterator<Polygon>(file),
              std::istream_iterator<Polygon>(),
              std::back_inserter(polygons));

    std::cout << std::fixed << std::setprecision(1);

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;
        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;

        try {
            if (cmd == "AREA") {
                std::string subcmd;
                iss >> subcmd;
                if (iss >> std::ws && !iss.eof()) throw std::invalid_argument("");

                if (subcmd == "EVEN") {
                    std::cout << std::accumulate(polygons.begin(), polygons.end(), 0.0, addIfEven) << '\n';
                } else if (subcmd == "ODD") {
                    std::cout << std::accumulate(polygons.begin(), polygons.end(), 0.0, addIfOdd) << '\n';
                } else if (subcmd == "MEAN") {
                    if (polygons.empty()) throw std::invalid_argument("");
                    double sum = std::accumulate(polygons.begin(), polygons.end(), 0.0, addArea);
                    std::cout << (sum / polygons.size()) << '\n';
                } else {
                    size_t count = parseStrictSizeT(subcmd);
                    if (count < 3) throw std::invalid_argument("");
                    std::cout << std::accumulate(polygons.begin(), polygons.end(), 0.0, std::bind(addIfSize, std::placeholders::_1, std::placeholders::_2, count)) << '\n';
                }
            } else if (cmd == "MAX") {
                std::string subcmd;
                iss >> subcmd;
                if (iss >> std::ws && !iss.eof()) throw std::invalid_argument("");
                if (polygons.empty()) throw std::invalid_argument("");

                if (subcmd == "AREA") {
                    auto it = std::max_element(polygons.begin(), polygons.end(), cmpArea);
                    std::cout << getArea(*it) << '\n';
                } else if (subcmd == "VERTEXES") {
                    auto it = std::max_element(polygons.begin(), polygons.end(), cmpVertices);
                    std::cout << it->points.size() << '\n';
                } else {
                    throw std::invalid_argument("");
                }
            } else if (cmd == "MIN") {
                std::string subcmd;
                iss >> subcmd;
                if (iss >> std::ws && !iss.eof()) throw std::invalid_argument("");
                if (polygons.empty()) throw std::invalid_argument("");

                if (subcmd == "AREA") {
                    auto it = std::min_element(polygons.begin(), polygons.end(), cmpArea);
                    std::cout << getArea(*it) << '\n';
                } else if (subcmd == "VERTEXES") {
                    auto it = std::min_element(polygons.begin(), polygons.end(), cmpVertices);
                    std::cout << it->points.size() << '\n';
                } else {
                    throw std::invalid_argument("");
                }
            } else if (cmd == "COUNT") {
                std::string subcmd;
                iss >> subcmd;
                if (iss >> std::ws && !iss.eof()) throw std::invalid_argument("");

                if (subcmd == "EVEN") {
                    std::cout << std::count_if(polygons.begin(), polygons.end(), isEvenVertices) << '\n';
                } else if (subcmd == "ODD") {
                    std::cout << std::count_if(polygons.begin(), polygons.end(), isOddVertices) << '\n';
                } else {
                    size_t count = parseStrictSizeT(subcmd);
                    if (count < 3) throw std::invalid_argument("");
                    std::cout << std::count_if(polygons.begin(), polygons.end(), std::bind(hasVertices, std::placeholders::_1, count)) << '\n';
                }
            } else if (cmd == "RMECHO") {
                Polygon target;
                if (!(iss >> target) || (iss >> std::ws && !iss.eof())) throw std::invalid_argument("");

                size_t initial_size = polygons.size();
                auto it = std::unique(polygons.begin(), polygons.end(), std::bind(isTargetPair, std::placeholders::_1, std::placeholders::_2, target));
                polygons.erase(it, polygons.end());
                std::cout << initial_size - polygons.size() << '\n';
            } else if (cmd == "INTERSECTIONS") {
                Polygon target;
                if (!(iss >> target) || (iss >> std::ws && !iss.eof())) throw std::invalid_argument("");

                std::cout << std::count_if(polygons.begin(), polygons.end(), std::bind(checkIntersect, std::placeholders::_1, target)) << '\n';
            } else {
                throw std::invalid_argument("");
            }
        } catch (...) {
            std::cout << "<INVALID COMMAND>\n";
        }
    }

    return 0;
}