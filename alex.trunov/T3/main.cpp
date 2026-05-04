#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <functional>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <istream>
#include <numeric>
#include <sstream>
#include <iterator>
#include <limits>
#include <string>
#include <vector>

using namespace std::placeholders;

// --- data structs ---

struct Point {
    int x, y;
    Point(int x = 0, int y = 0) :
        x(x), y(y)
    { }
};

struct Polygon {
    std::vector<Point> points;

    Polygon() = default;
    explicit Polygon(std::size_t n) : points(n) {}
};

struct BBox {
    int minX, minY, maxX, maxY;
};

// --- ios operators ---

std::istream& operator>>(std::istream& is, Point& dest) {
    std::istream::sentry sentry(is);
    if (!sentry)    return is;

    char c1, c2, c3;
    int x, y;
    if ((is >> c1 >> x >> c2 >> y >> c3) &&
        (c1 == '(' && c2 == ';' && c3 == ')')) {
        dest = { x, y };
    }
    else {
        is.setstate(std::ios::failbit);
    }

    return is;
}

std::istream& operator>>(std::istream& is, Polygon& dest) {
    std::istream::sentry sentry(is);
    if (!sentry)    return is;

    size_t n;
    if (!(is >> n) || n < 3) {
        is.setstate(std::ios::failbit);
        return is;
    }

    Polygon tmp(n);
    std::copy_n(std::istream_iterator<Point>(is), n, tmp.points.begin());

    if (is)            dest = std::move(tmp);
    return is;
}

// --- find area ---

struct CrossProduct {
    const std::vector<Point>& p;
    mutable size_t i;
    explicit CrossProduct(const std::vector<Point>& points) :
        p(points), i(0)
    { }

    long long operator()(long long acc, const Point& cur) {
        const Point& next = p[(i + 1) % p.size()];
        long long res = static_cast<long long>(cur.x) * next.y
            - static_cast<long long>(next.x) * cur.y;
        i++;
        return acc + res;
    }
};

double area(const Polygon& p) {
    long long sum = std::accumulate(p.points.begin(),
        p.points.end(), 0ll, CrossProduct(p.points));

    return std::abs(sum) * 0.5;
}

// --- common functors ---

struct SumArea {
    double operator()(double acc, const Polygon& p) const {
        return acc + area(p);
    }
};

struct IsEven {
    bool operator()(const Polygon& p) const {
        return p.points.size() % 2 == 0;
    }
};

struct SumIfEven {
    double operator()(double acc, const Polygon& p) const {
        return IsEven()(p) ? acc + area(p) : acc;
    }
};

struct SumIfOdd {
    double operator()(double acc, const Polygon& p) {
        return !IsEven()(p) ? acc + area(p) : acc;
    }
};

struct AreaIfVertexCount {
    size_t n;
    explicit AreaIfVertexCount(size_t n) : n(n) { }

    double operator()(double acc, const Polygon& p) {
        return (p.points.size() == n) ? acc + area(p) : acc;
    }
};

struct AreaCompare {
    bool operator()(const Polygon& a, const Polygon& b) {
        return area(a) < area(b);
    }
};

struct NumOfVertexesCompare {
    bool operator()(const Polygon& a, const Polygon& b) {
        return a.points.size() < b.points.size();
    }
};

struct IsNumOfVertexes {
    size_t n;
    explicit IsNumOfVertexes(size_t n) : n(n) { }

    bool operator()(const Polygon& p) {
        return p.points.size() == n;
    }
};

// --- PERMS functors ---

struct PointLess {
    bool operator()(const Point& a, const Point& b) {
        return (a.x != b.x) ? (a.x < b.x) : (a.y < b.y);
    }
};

struct PointEq {
    bool operator()(const Point& a, const Point& b) {
        return a.x == b.x && a.y == b.y;
    }
};

std::vector<Point> sortPoints(const Polygon& p) {
    std::vector<Point> pts = p.points;
    std::sort(pts.begin(), pts.end(), PointLess());
    return pts;
}

struct IsPermutation {
    const std::vector<Point>& p;
    explicit IsPermutation(const std::vector<Point>& point) : p(point) { }

    bool operator()(const Polygon& pol) {
        std::vector<Point> pts = pol.points;
        if (pts.size() != p.size())
            return false;

        std::sort(pts.begin(), pts.end(), PointLess());
        return std::equal(pts.begin(), pts.end(), p.begin(), PointEq());
    }
};

// --- INFRAME functors ---

struct ExpandingBBoxPoints {
    BBox operator()(BBox b, const Point& p) const {
        b.minX = std::min(b.minX, p.x);
        b.maxX = std::max(b.maxX, p.x);
        b.minY = std::min(b.minY, p.y);
        b.maxY = std::max(b.maxY, p.y);
        return b;
    }
};

struct ExpandingBBoxPolygons {
    BBox operator()(BBox b, const Polygon& p) const {
        std::vector<Point> pnt = p.points;
        return std::accumulate(pnt.begin(), pnt.end(),
            b, ExpandingBBoxPoints());
    }
};

struct PointInFrame {
    const BBox& b;
    explicit PointInFrame(const BBox& bb) : b(bb) { }

    bool operator()(const Point& p) const {
        return p.x >= b.minX && p.x < b.maxX
            && p.y >= b.minY && p.y < b.maxY;
    }
};

BBox constructBB(const std::vector<Polygon>& pol) {
    BBox init{ std::numeric_limits<int>::max(),
    std::numeric_limits<int>::max(),
    std::numeric_limits<int>::min(),
    std::numeric_limits<int>::min() };
    return std::accumulate(pol.begin(), pol.end(),
        init, ExpandingBBoxPolygons());
}

// !!! --- main --- !!!

int main(int argc, char* argv[]) {

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>\n";
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file) {
        std::cerr << "Error: cannot open file " << argv[1] << "\n";
        return 1;
    }

    std::vector<Polygon> polygons;
    std::string raw;
    while (std::getline(file, raw)) {
        if (raw.empty()) continue;
        std::istringstream iss(raw);
        Polygon poly;
        if (iss >> poly) {
            iss >> std::ws;
            if (iss.eof()) polygons.push_back(std::move(poly));
        }
    }

    std::cout << std::fixed << std::setprecision(1);

    std::string command;
    while (std::getline(std::cin, command)) {

        if (command.empty()) continue;
        std::istringstream iss(command);
        std::string mainСmd;
        iss >> mainСmd;

        // === for AREA ===

        if (mainСmd == "AREA") {
            std::string sub;
            iss >> sub;

            if (sub == "EVEN") {
                std::cout << std::accumulate(polygons.begin(), polygons.end(),
                    0.0, SumIfEven()) << "\n";
            }
            else if (sub == "ODD") {
                std::cout << std::accumulate(polygons.begin(), polygons.end(),
                    0.0, SumIfOdd()) << "\n";
            }
            else if (sub == "MEAN") {
                if (polygons.empty()) {
                    std::cout << "<INVALID COMMAND>\n";
                }
                else {
                    double total = std::accumulate(polygons.begin(),
                        polygons.end(), 0.0,
                        SumArea());
                    std::cout << total / polygons.size() << "\n";
                }
            }

            else {
                std::istringstream num_stream(sub);
                std::size_t n;
                if (num_stream >> n && num_stream.eof() && n >= 3) {
                    std::cout << std::accumulate(polygons.begin(),
                        polygons.end(), 0.0,
                        AreaIfVertexCount(n)) << "\n";
                }
                else {
                    std::cout << "<INVALID COMMAND>\n";
                }
            }
        }

        // === for MAX ===

        else if (mainСmd == "MAX") {
            std::string sub;
            iss >> sub;
            if (polygons.empty()) {
                std::cout << "<INVALID COMMAND>\n";
            }
            else if (sub == "AREA") {
                auto it = std::max_element(polygons.begin(), polygons.end(),
                    AreaCompare());
                std::cout << area(*it) << "\n";
            }
            else if (sub == "VERTEXES") {
                auto it = std::max_element(polygons.begin(), polygons.end(),
                    NumOfVertexesCompare());
                std::cout << it->points.size() << "\n";
            }

            else {
                std::cout << "<INVALID COMMAND>\n";
            }
        }

        // === for MIN ===

        else if (mainСmd == "MIN") {
            std::string sub;
            iss >> sub;

            if (polygons.empty()) {
                std::cout << "<INVALID COMMAND>\n";
            }
            else if (sub == "AREA") {
                auto it = std::min_element(polygons.begin(), polygons.end(),
                    AreaCompare());
                std::cout << area(*it) << "\n";
            }
            else if (sub == "VERTEXES") {
                auto it = std::min_element(polygons.begin(), polygons.end(),
                    NumOfVertexesCompare());
                std::cout << it->points.size() << "\n";
            }

            else {
                std::cout << "<INVALID COMMAND>\n";
            }
        }

        // === for COUNT ===

        else if (mainСmd == "COUNT") {
            std::string sub;
            iss >> sub;

            if (sub == "EVEN") {
                std::cout << std::count_if(polygons.begin(), polygons.end(),
                    IsEven()) << "\n";
            }
            else if (sub == "ODD") {
                auto odd_pred = std::bind(std::logical_not<bool>(),
                    std::bind(IsEven(), _1));
                std::cout << std::count_if(polygons.begin(), polygons.end(),
                    odd_pred) << "\n";
            }

            else {
                std::istringstream num_stream(sub);
                std::size_t n;
                if (num_stream >> n && num_stream.eof() && n >= 3) {
                    std::cout << std::count_if(polygons.begin(), polygons.end(),
                        IsNumOfVertexes(n)) << "\n";
                }
                else {
                    std::cout << "<INVALID COMMAND>\n";
                }
            }
        }

        // === for PERMS ===

        else if (mainСmd == "PERMS") {
            Polygon target;
            if (!(iss >> target)) {
                std::cout << "<INVALID COMMAND>\n";
            }
            else {
                std::vector<Point> st = sortPoints(target);
                std::cout << std::count_if(polygons.begin(), polygons.end(),
                    IsPermutation(st)) << "\n";
            }
        }

        // === for INFRAME ===

        else if (mainСmd == "INFRAME") {
            Polygon target;

            if (!(iss >> target)) {
                std::cout << "<INVALID COMMAND>\n";
            }
            else if (polygons.empty()) {
                std::cout << "<FALSE>\n";
            }
            else {
                BBox bb = constructBB(polygons);
                bool inside = std::all_of(target.points.begin(),
                    target.points.end(),
                    PointInFrame(bb));
                std::cout << (inside ? "<TRUE>" : "<FALSE>") << "\n";
            }
        }

        else {
            std::cout << "<INVALID COMMAND>\n";
        }
    }

    return 0;
}
