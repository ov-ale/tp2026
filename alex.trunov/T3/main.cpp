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
    {
    }
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

    if (is)         dest = std::move(tmp);
    return is;
}

// --- find area ---
struct CrossProduct {
    const std::vector<Point>& p;
    mutable size_t i;
    explicit CrossProduct(const std::vector<Point>& points) :
        p(points), i(0)
    {
    }

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
    double operator()(double acc, const Polygon& p) const {
        return !IsEven()(p) ? acc + area(p) : acc;
    }
};

struct AreaIfVertexCount {
    size_t n;
    explicit AreaIfVertexCount(size_t n) : n(n) {}

    double operator()(double acc, const Polygon& p) const {
        return (p.points.size() == n) ? acc + area(p) : acc;
    }
};

struct AreaCompare {
    bool operator()(const Polygon& a, const Polygon& b) const {
        return area(a) < area(b);
    }
};

struct NumOfVertexesCompare {
    bool operator()(const Polygon& a, const Polygon& b) const {
        return a.points.size() < b.points.size();
    }
};

struct IsNumOfVertexes {
    size_t n;
    explicit IsNumOfVertexes(size_t n) : n(n) {}

    bool operator()(const Polygon& p) const {
        return p.points.size() == n;
    }
};

// --- LESSAREA functors ---
struct PointEq {
    bool operator()(const Point& a, const Point& b) const {
        return a.x == b.x && a.y == b.y;
    }
};

struct IsLessArea {
    const Polygon& pol;
    explicit IsLessArea(const Polygon& p) : pol(p) {}

    bool operator()(const Polygon& p) const {
        return area(p) < area(pol);
    }
};

// --- SAME functors ---
struct PointDiff {
    Point operator()(const Point& a, const Point& b) const {
        return Point(a.x - b.x, a.y - b.y);
    }
};

std::vector<Point> toCanonical(const Polygon& p) {
    std::vector<Point> res(p.points.size());

    std::transform(p.points.begin(), p.points.end(),
        res.begin(),
        std::bind(PointDiff(), _1, std::cref(p.points.front())));
    return res;
}

struct IsSame {
    const std::vector<Point>& normTarget;
    explicit IsSame(const std::vector<Point>& nt) : normTarget(nt) {}

    bool operator()(const Polygon& p) const {
        if (p.points.size() != normTarget.size()) return false;
        std::vector<Point> norm = toCanonical(p);
        return std::equal(norm.begin(), norm.end(),
            normTarget.begin(), PointEq());
    }
};

// !!! --- main --- !!!

int main(int argc, char* argv[]) {

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>\n";
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file) {
        std::cerr << "Error: can not open file " << argv[1] << '\n';
        return 1;
    }

    std::vector<Polygon> polygons;
    std::string raw;
    while (std::getline(file, raw)) {
        if (raw.empty()) continue;
        std::istringstream iss(raw);
        Polygon pol;
        if (iss >> pol) {
            iss >> std::ws;
            if (iss.eof())
                polygons.push_back(std::move(pol));
        }
    }

    // === inputing comands ===
    std::cout << std::fixed << std::setprecision(1);

    std::string command;
    while (std::getline(std::cin, command)) {

        if (command.empty())
            continue;
        std::istringstream iss(command);
        std::string mainCmd;
        iss >> mainCmd;

        // === for AREA ===
        if (mainCmd == "AREA") {
            std::string sub;
            iss >> sub;

            if (sub == "EVEN") {
                std::cout << std::accumulate(polygons.begin(), polygons.end(),
                    0.0, SumIfEven()) << '\n';
            }
            else if (sub == "ODD") {
                std::cout << std::accumulate(polygons.begin(), polygons.end(),
                    0.0, SumIfOdd()) << '\n';
            }
            else if (sub == "MEAN") {
                if (!polygons.empty()) {
                    double total = std::accumulate(polygons.begin(),
                        polygons.end(), 0.0, SumArea());
                    std::cout << total / polygons.size() << '\n';
                }
                else {
                    std::cout << "<INVALID COMMAND>\n";
                }
            }
            else {
                std::istringstream numStream(sub);
                size_t n;
                if (numStream >> n && numStream.eof() && n >= 3) {
                    std::cout << std::accumulate(polygons.begin(),
                        polygons.end(), 0.0, AreaIfVertexCount(n)) << '\n';
                }
                else {
                    std::cout << "<INVALID COMMAND>\n";
                }
            }
        }

        // === for MAX ===
        else if (mainCmd == "MAX") {
            std::string sub;
            iss >> sub;

            if (polygons.empty()) {
                std::cout << "<INVALID COMMAND>\n";
            }
            else if (sub == "AREA") {
                auto it = std::max_element(polygons.begin(), polygons.end(),
                    AreaCompare());
                std::cout << area(*it) << '\n';
            }
            else if (sub == "VERTEXES") {
                auto it = std::max_element(polygons.begin(), polygons.end(),
                    NumOfVertexesCompare());
                std::cout << (*it).points.size() << '\n';
            }
            else {
                std::cout << "<INVALID COMMAND>\n";
            }
        }

        // === for MIN ===
        else if (mainCmd == "MIN") {
            std::string sub;
            iss >> sub;

            if (polygons.empty()) {
                std::cout << "<INVALID COMMAND>\n";
            }
            else if (sub == "AREA") {
                auto it = std::min_element(polygons.begin(), polygons.end(),
                    AreaCompare());
                std::cout << area(*it) << '\n';
            }
            else if (sub == "VERTEXES") {
                auto it = std::min_element(polygons.begin(), polygons.end(),
                    NumOfVertexesCompare());
                std::cout << (*it).points.size() << '\n';
            }
            else {
                std::cout << "<INVALID COMMAND>\n";
            }
        }

        // === for COUNT ===
        else if (mainCmd == "COUNT") {
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

        // === for LESSAREA ===
        else if (mainCmd == "LESSAREA") {
            Polygon target;
            if (!(iss >> target)) {
                std::cout << "<INVALID COMMAND>\n";
            }
            else {
                iss >> std::ws;
                if (!iss.eof()) {
                    std::cout << "<INVALID COMMAND>\n";
                }
                else {
                    std::cout << std::count_if(polygons.begin(), polygons.end(),
                        IsLessArea(target)) << "\n";
                }
            }
        }

        // === for SAME ===
        else if (mainCmd == "SAME") {
            Polygon target;
            if (!(iss >> target)) {
                std::cout << "<INVALID COMMAND>\n";
            }
            else {
                iss >> std::ws;
                if (!iss.eof()) {
                    std::cout << "<INVALID COMMAND>\n";
                }
                else {
                    std::vector<Point> nt = toCanonical(target);
                    std::cout << std::count_if(polygons.begin(), polygons.end(),
                        IsSame(nt)) << "\n";
                }
            }
        }

        else {
            std::cout << "<INVALID COMMAND>\n";
        }
    }

    return 0;
}
