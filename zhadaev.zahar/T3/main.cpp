#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <functional>
#include <iterator>
#include <iomanip>
#include <cmath>

using namespace std::placeholders;

struct Point {
    int x, y;
};

struct Polygon {
    std::vector<Point> points;
};

double getArea(const Polygon& poly) {
    if (poly.points.size() < 3) return 0.0;
    double area = std::accumulate(
        poly.points.begin(), 
        std::prev(poly.points.end()), 
        0.0,
        [](double sum, const Point& curr) {
            const Point& next = *(&curr + 1);
            return sum + (curr.x * next.y - next.x * curr.y);
        }
    );
    area += (poly.points.back().x * poly.points.front().y - poly.points.front().x * poly.points.back().y);
    return std::abs(area) / 2.0;
}

bool areEqual(const Polygon& a, const Polygon& b) {
    if (a.points.size() != b.points.size()) return false;
    return std::equal(a.points.begin(), a.points.end(), b.points.begin(), 
        [](const Point& p1, const Point& p2) {
            return p1.x == p2.x && p1.y == p2.y;
        });
}

bool intersect(const Polygon& a, const Polygon& b) {
    auto getBounds = [](const Polygon& p) {
        auto minMaxX = std::minmax_element(p.points.begin(), p.points.end(), [](const Point& a, const Point& b) { return a.x < b.x; });
        auto minMaxY = std::minmax_element(p.points.begin(), p.points.end(), [](const Point& a, const Point& b) { return a.y < b.y; });
        return std::make_pair(std::make_pair(minMaxX.first->x, minMaxX.second->x), std::make_pair(minMaxY.first->y, minMaxY.second->y));
    };
    auto b1 = getBounds(a), b2 = getBounds(b);
    return !(b1.first.second < b2.first.first || b2.first.second < b1.first.first ||
             b1.second.second < b2.second.first || b2.second.second < b1.second.first);
}

std::istream& operator>>(std::istream& in, Point& p) {
    char ignore;
    if (in >> ignore && ignore == '(' && in >> p.x >> ignore && ignore == ';' && in >> p.y >> ignore && ignore == ')') {
        return in;
    }
    in.setstate(std::ios::failbit);
    return in;
}

std::istream& operator>>(std::istream& in, Polygon& poly) {
    size_t n;
    if (!(in >> n)) return in;
    poly.points.clear();
    std::copy_n(std::istream_iterator<Point>(in), n, std::back_inserter(poly.points));
    return in;
}

void handleArea(const std::vector<Polygon>& shapes) {
    std::string arg;
    std::cin >> arg;
    double result = 0;
    if (arg == "EVEN") {
        result = std::accumulate(shapes.begin(), shapes.end(), 0.0, [](double sum, const Polygon& p) {
            return (p.points.size() % 2 == 0) ? sum + getArea(p) : sum;
        });
    } else if (arg == "ODD") {
        result = std::accumulate(shapes.begin(), shapes.end(), 0.0, [](double sum, const Polygon& p) {
            return (p.points.size() % 2 != 0) ? sum + getArea(p) : sum;
        });
    } else if (arg == "MEAN" && !shapes.empty()) {
        result = std::accumulate(shapes.begin(), shapes.end(), 0.0, [](double sum, const Polygon& p) {
            return sum + getArea(p);
        }) / shapes.size();
    } else if (std::isdigit(arg[0])) {
        size_t n = std::stoul(arg);
        result = std::accumulate(shapes.begin(), shapes.end(), 0.0, [n](double sum, const Polygon& p) {
            return (p.points.size() == n) ? sum + getArea(p) : sum;
        });
    } else {
        std::cout << "<INVALID COMMAND>" << std::endl;
        return;
    }
    std::cout << std::fixed << std::setprecision(1) << result << std::endl;
}

void handleRmEcho(std::vector<Polygon>& shapes) {
    Polygon target;
    if (!(std::cin >> target)) return;
    size_t oldSize = shapes.size();
    auto it = std::unique(shapes.begin(), shapes.end(), [&target](const Polygon& a, const Polygon& b) {
        return areEqual(a, target) && areEqual(b, target);
    });
    shapes.erase(it, shapes.end());
    std::cout << oldSize - shapes.size() << std::endl;
}

void handleIntersections(const std::vector<Polygon>& shapes) {
    Polygon target;
    if (!(std::cin >> target)) return;
    auto count = std::count_if(shapes.begin(), shapes.end(), std::bind(intersect, _1, std::cref(target)));
    std::cout << count << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) return 1;
    std::ifstream file(argv[1]);
    std::vector<Polygon> shapes;
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        Polygon p;
        if (ss >> p) shapes.push_back(p);
    }
    std::string cmd;
    while (std::cin >> cmd) {
        if (cmd == "AREA") handleArea(shapes);
        else if (cmd == "RMECHO") handleRmEcho(shapes);
        else if (cmd == "INTERSECTIONS") handleIntersections(shapes);
        else {
            std::cout << "<INVALID COMMAND>" << std::endl;
            std::cin.ignore(1000, '\n');
        }
    }
    return 0;
}
