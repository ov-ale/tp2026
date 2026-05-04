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
    return std::equal(a.points.begin(), a.points.end(), b.points.begin(),
        [](const Point& p1, const Point& p2) {
            return p1.x == p2.x && p1.y == p2.y;
        });
}

bool intersect(const Polygon& a, const Polygon& b) {
    auto getBounds = [](const Polygon& p) {
        auto cmpX = [](const Point& p1, const Point& p2) { return p1.x < p2.x; };
        auto cmpY = [](const Point& p1, const Point& p2) { return p1.y < p2.y; };
        auto mmX = std::minmax_element(p.points.begin(), p.points.end(), cmpX);
        auto mmY = std::minmax_element(p.points.begin(), p.points.end(), cmpY);
        return std::make_pair(std::make_pair(mmX.first->x, mmX.second->x),
            std::make_pair(mmY.first->y, mmY.second->y));
        };
    auto b1 = getBounds(a), b2 = getBounds(b);
    bool noX = b1.first.second < b2.first.first || b2.first.second < b1.first.first;
    bool noY = b1.second.second < b2.second.first || b2.second.second < b1.second.first;
    return !(noX || noY);
}

std::istream& operator>>(std::istream& in, Point& p) {
    char c1, c2, c3;
    if (in >> c1 >> p.x >> c2 >> p.y >> c3 && c1 == '(' && c2 == ';' && c3 == ')') return in;
    in.setstate(std::ios::failbit);
    return in;
}

std::istream& operator>>(std::istream& in, Polygon& poly) {
    size_t n;
    if (!(in >> n) || n < 3) {
        in.setstate(std::ios::failbit);
        return in;
    }
    poly.points.clear();
    std::copy_n(std::istream_iterator<Point>(in), n, std::back_inserter(poly.points));
    return in;
}

void handleCount(const std::vector<Polygon>& shapes) {
    std::string arg;
    std::cin >> arg;
    if (arg == "EVEN") {
        std::cout << std::count_if(shapes.begin(), shapes.end(),
            [](const Polygon& p) { return p.points.size() % 2 == 0; }) << std::endl;
    }
    else if (arg == "ODD") {
        std::cout << std::count_if(shapes.begin(), shapes.end(),
            [](const Polygon& p) { return p.points.size() % 2 != 0; }) << std::endl;
    }
    else if (std::isdigit(arg[0])) {
        size_t n = std::stoul(arg);
        if (n < 3) { std::cout << "<INVALID COMMAND>" << std::endl; return; }
        std::cout << std::count_if(shapes.begin(), shapes.end(),
            [n](const Polygon& p) { return p.points.size() == n; }) << std::endl;
    }
    else std::cout << "<INVALID COMMAND>" << std::endl;
}

void handleMax(const std::vector<Polygon>& shapes) {
    if (shapes.empty()) { std::cout << "<INVALID COMMAND>" << std::endl; return; }
    std::string arg;
    std::cin >> arg;
    if (arg == "AREA") {
        auto it = std::max_element(shapes.begin(), shapes.end(),
            [](const Polygon& a, const Polygon& b) { return getArea(a) < getArea(b); });
        std::cout << std::fixed << std::setprecision(1) << getArea(*it) << std::endl;
    }
    else if (arg == "VERTEXES") {
        auto it = std::max_element(shapes.begin(), shapes.end(),
            [](const Polygon& a, const Polygon& b) { return a.points.size() < b.points.size(); });
        std::cout << it->points.size() << std::endl;
    }
    else std::cout << "<INVALID COMMAND>" << std::endl;
}

void handleArea(const std::vector<Polygon>& shapes) {
    std::string arg;
    std::cin >> arg;
    if (arg == "EVEN") {
        std::cout << std::fixed << std::setprecision(1) << std::accumulate(shapes.begin(), shapes.end(), 0.0,
            [](double s, const Polygon& p) { return p.points.size() % 2 == 0 ? s + getArea(p) : s; }) << std::endl;
    }
    else if (arg == "ODD") {
        std::cout << std::fixed << std::setprecision(1) << std::accumulate(shapes.begin(), shapes.end(), 0.0,
            [](double s, const Polygon& p) { return p.points.size() % 2 != 0 ? s + getArea(p) : s; }) << std::endl;
    }
    else if (arg == "MEAN" && !shapes.empty()) {
        double total = std::accumulate(shapes.begin(), shapes.end(), 0.0,
            [](double s, const Polygon& p) { return s + getArea(p); });
        std::cout << std::fixed << std::setprecision(1) << total / shapes.size() << std::endl;
    }
    else if (std::isdigit(arg[0])) {
        size_t n = std::stoul(arg);
        if (n < 3) { std::cout << "<INVALID COMMAND>" << std::endl; return; }
        std::cout << std::fixed << std::setprecision(1) << std::accumulate(shapes.begin(), shapes.end(), 0.0,
            [n](double s, const Polygon& p) { return p.points.size() == n ? s + getArea(p) : s; }) << std::endl;
    }
    else std::cout << "<INVALID COMMAND>" << std::endl;
}

void handleIntersections(const std::vector<Polygon>& shapes) {
    Polygon target;
    if (!(std::cin >> target)) {
        std::cout << "<INVALID COMMAND>" << std::endl;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }
    std::cout << std::count_if(shapes.begin(), shapes.end(),
        std::bind(intersect, _1, std::cref(target))) << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc != 2) { std::cerr << "Usage: " << argv[0] << " <file>" << std::endl; return 1; }
    std::ifstream file(argv[1]);
    std::vector<Polygon> shapes;
    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        Polygon p;
        if (ss >> p) shapes.push_back(p);
    }
    std::string cmd;
    while (std::cin >> cmd) {
        if (cmd == "AREA") handleArea(shapes);
        else if (cmd == "COUNT") handleCount(shapes);
        else if (cmd == "MAX") handleMax(shapes);
        else if (cmd == "INTERSECTIONS") handleIntersections(shapes);
        else if (cmd == "RMECHO") {
            Polygon t;
            if (std::cin >> t) {
                size_t s = shapes.size();
                auto it = std::unique(shapes.begin(), shapes.end(),
                    [&t](const Polygon& a, const Polygon& b) { return areEqual(a, t) && areEqual(b, t); });
                shapes.erase(it, shapes.end());
                std::cout << s - shapes.size() << std::endl;
            }
        }
        else {
            std::cout << "<INVALID COMMAND>" << std::endl;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
    return 0;
}
