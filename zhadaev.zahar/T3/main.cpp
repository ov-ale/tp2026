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
#include <limits>

using namespace std::placeholders;

struct Point {
    int x, y;
};

struct Polygon {
    std::vector<Point> points;
};

double getArea(const Polygon& poly) {
    if (poly.points.size() < 3) return 0.0;
    auto begin = poly.points.begin();
    auto end_prev = std::prev(poly.points.end());
    
    double area = std::accumulate(
        begin,
        end_prev,
        0.0,
        [](double sum, const Point& curr) {
            const Point& next = *(&curr + 1);
            double dx = static_cast<double>(curr.x) * next.y;
            double dy = static_cast<double>(next.x) * curr.y;
            return sum + (dx - dy);
        }
    );
    
    double last_x = static_cast<double>(poly.points.back().x);
    double last_y = static_cast<double>(poly.points.front().y);
    double first_x = static_cast<double>(poly.points.front().x);
    double first_y = static_cast<double>(poly.points.back().y);
    
    area += (last_x * last_y - first_x * first_y);
    return std::abs(area) / 2.0;
}

bool areEqual(const Polygon& a, const Polygon& b) {
    if (a.points.size() != b.points.size()) return false;
    auto check = [](const Point& p1, const Point& p2) {
        return p1.x == p2.x && p1.y == p2.y;
    };
    return std::equal(a.points.begin(), a.points.end(), b.points.begin(), check);
}

bool intersect(const Polygon& a, const Polygon& b) {
    auto getBounds = [](const Polygon& p) {
        auto cmpX = [](const Point& p1, const Point& p2) { return p1.x < p2.x; };
        auto cmpY = [](const Point& p1, const Point& p2) { return p1.y < p2.y; };
        auto mmX = std::minmax_element(p.points.begin(), p.points.end(), cmpX);
        auto mmY = std::minmax_element(p.points.begin(), p.points.end(), cmpY);
        return std::make_pair(
            std::make_pair(mmX.first->x, mmX.second->x),
            std::make_pair(mmY.first->y, mmY.second->y)
        );
    };
    auto b1 = getBounds(a), b2 = getBounds(b);
    bool noX = b1.first.second < b2.first.first || b2.first.second < b1.first.first;
    bool noY = b1.second.second < b2.second.first || b2.second.second < b1.second.first;
    return !(noX || noY);
}

std::istream& operator>>(std::istream& in, Point& p) {
    char c1, c2, c3;
    if (in >> c1 >> p.x >> c2 >> p.y >> c3 && c1 == '(' && c2 == ';' && c3 == ')') {
        return in;
    }
    in.setstate(std::ios::failbit);
    return in;
}

std::istream& operator>>(std::istream& in, Polygon& poly) {
    size_t n;
    if (!(in >> n)) return in;
    poly.points.clear();
    auto it = std::back_inserter(poly.points);
    std::copy_n(std::istream_iterator<Point>(in), n, it);
    return in;
}

void handleArea(const std::vector<Polygon>& shapes) {
    std::string arg;
    std::cin >> arg;
    double result = 0;
    if (arg == "EVEN") {
        result = std::accumulate(shapes.begin(), shapes.end(), 0.0, 
            [](double sum, const Polygon& p) {
                return (p.points.size() % 2 == 0) ? sum + getArea(p) : sum;
            });
    } else if (arg == "ODD") {
        result = std::accumulate(shapes.begin(), shapes.end(), 0.0, 
            [](double sum, const Polygon& p) {
                return (p.points.size() % 2 != 0) ? sum + getArea(p) : sum;
            });
    } else if (arg == "MEAN" && !shapes.empty()) {
        double total = std::accumulate(shapes.begin(), shapes.end(), 0.0, 
            [](double sum, const Polygon& p) {
                return sum + getArea(p);
            });
        result = total / static_cast<double>(shapes.size());
    } else if (!arg.empty() && std::isdigit(arg[0])) {
        size_t n = std::stoul(arg);
        result = std::accumulate(shapes.begin(), shapes.end(), 0.0, 
            [n](double sum, const Polygon& p) {
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
    auto pred = [&target](const Polygon& a, const Polygon& b) {
        return areEqual(a, target) && areEqual(b, target);
    };
    auto it = std::unique(shapes.begin(), shapes.end(), pred);
    shapes.erase(it, shapes.end());
    std::cout << oldSize - shapes.size() << std::endl;
}

void handleIntersections(const std::vector<Polygon>& shapes) {
    Polygon target;
    if (!(std::cin >> target)) return;
    auto fn = std::bind(intersect, _1, std::cref(target));
    auto count = std::count_if(shapes.begin(), shapes.end(), fn);
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
            auto max_s = std::numeric_limits<std::streamsize>::max();
            std::cin.ignore(max_s, '\n');
        }
    }
    return 0;
}
