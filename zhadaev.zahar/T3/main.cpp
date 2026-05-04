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
    double area = 0.0;
    for (size_t i = 0; i < poly.points.size(); ++i) {
        const Point& curr = poly.points[i];
        const Point& next = poly.points[(i + 1) % poly.points.size()];
        area += (static_cast<double>(curr.x) * next.y -
            static_cast<double>(next.x) * curr.y);
    }
    return std::abs(area) / 2.0;
}

bool intersect(const Polygon& a, const Polygon& b) {
    auto getBounds = [](const Polygon& p) {
        auto mmX = std::minmax_element(p.points.begin(), p.points.end(),
            [](const Point& p1, const Point& p2) { return p1.x < p2.x; });
        auto mmY = std::minmax_element(p.points.begin(), p.points.end(),
            [](const Point& p1, const Point& p2) { return p1.y < p2.y; });
        return std::make_pair(
            std::make_pair(mmX.first->x, mmX.second->x),
            std::make_pair(mmY.first->y, mmY.second->y)
        );
        };
    auto b1 = getBounds(a), b2 = getBounds(b);
    return !(b1.first.second < b2.first.first ||
        b2.first.second < b1.first.first ||
        b1.second.second < b2.second.first ||
        b2.second.second < b1.second.first);
}

std::istream& operator>>(std::istream& in, Point& p) {
    char c1, c2, c3;
    int x, y;
    if (in >> std::ws >> c1 && c1 == '(' &&
        in >> x >> c2 && c2 == ';' &&
        in >> y >> c3 && c3 == ')') {
        p.x = x; p.y = y;
        return in;
    }
    in.setstate(std::ios::failbit);
    return in;
}

std::istream& operator>>(std::istream& in, Polygon& poly) {
    size_t n;
    if (!(in >> n)) return in;
    std::vector<Point> temp_points;
    for (size_t i = 0; i < n; ++i) {
        Point p;
        if (!(in >> p)) {
            in.setstate(std::ios::failbit);
            return in;
        }
        temp_points.push_back(p);
    }
    poly.points = std::move(temp_points);
    return in;
}

void skipUntilNewline() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void handleArea(const std::vector<Polygon>& shapes) {
    std::string arg;
    if (!(std::cin >> arg)) return;
    if (arg == "EVEN") {
        double res = std::accumulate(shapes.begin(), shapes.end(), 0.0,
            [](double s, const Polygon& p) {
                return p.points.size() % 2 == 0 ? s + getArea(p) : s;
            });
        std::cout << std::fixed << std::setprecision(1) << res << "\n";
    }
    else if (arg == "ODD") {
        double res = std::accumulate(shapes.begin(), shapes.end(), 0.0,
            [](double s, const Polygon& p) {
                return p.points.size() % 2 != 0 ? s + getArea(p) : s;
            });
        std::cout << std::fixed << std::setprecision(1) << res << "\n";
    }
    else if (arg == "MEAN") {
        if (shapes.empty()) {
            std::cout << "<INVALID COMMAND>\n";
            return;
        }
        double total = std::accumulate(shapes.begin(), shapes.end(), 0.0,
            [](double s, const Polygon& p) { return s + getArea(p); });
        std::cout << std::fixed << std::setprecision(1) << total / shapes.size() << "\n";
    }
    else if (std::isdigit(arg[0])) {
        size_t n = std::stoul(arg);
        if (n < 3) { std::cout << "<INVALID COMMAND>\n"; return; }
        double res = std::accumulate(shapes.begin(), shapes.end(), 0.0,
            [n](double s, const Polygon& p) {
                return p.points.size() == n ? s + getArea(p) : s;
            });
        std::cout << std::fixed << std::setprecision(1) << res << "\n";
    }
    else {
        std::cout << "<INVALID COMMAND>\n";
    }
}

void handleMax(const std::vector<Polygon>& shapes) {
    std::string arg;
    if (!(std::cin >> arg)) return;
    if (shapes.empty()) {
        std::cout << "<INVALID COMMAND>\n";
        return;
    }
    if (arg == "AREA") {
        auto it = std::max_element(shapes.begin(), shapes.end(),
            [](const Polygon& a, const Polygon& b) {
                return getArea(a) < getArea(b);
            });
        std::cout << std::fixed << std::setprecision(1) << getArea(*it) << "\n";
    }
    else if (arg == "VERTEXES") {
        auto it = std::max_element(shapes.begin(), shapes.end(),
            [](const Polygon& a, const Polygon& b) {
                return a.points.size() < b.points.size();
            });
        std::cout << it->points.size() << "\n";
    }
    else {
        std::cout << "<INVALID COMMAND>\n";
    }
}

void handleCount(const std::vector<Polygon>& shapes) {
    std::string arg;
    if (!(std::cin >> arg)) return;
    if (arg == "EVEN") {
        std::cout << std::count_if(shapes.begin(), shapes.end(),
            [](const Polygon& p) { return p.points.size() % 2 == 0; }) << "\n";
    }
    else if (arg == "ODD") {
        std::cout << std::count_if(shapes.begin(), shapes.end(),
            [](const Polygon& p) { return p.points.size() % 2 != 0; }) << "\n";
    }
    else if (std::isdigit(arg[0])) {
        size_t n = std::stoul(arg);
        if (n < 3) { std::cout << "<INVALID COMMAND>\n"; return; }
        std::cout << std::count_if(shapes.begin(), shapes.end(),
            [n](const Polygon& p) { return p.points.size() == n; }) << "\n";
    }
    else {
        std::cout << "<INVALID COMMAND>\n";
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) return 1;
    std::ifstream file(argv[1]);
    std::vector<Polygon> shapes;
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        Polygon p;
        if (ss >> p) {
            std::string extra;
            if (!(ss >> extra)) shapes.push_back(p);
        }
    }

    std::string cmd;
    while (std::cin >> cmd) {
        if (cmd == "AREA") handleArea(shapes);
        else if (cmd == "MAX") handleMax(shapes);
        else if (cmd == "COUNT") handleCount(shapes);
        else if (cmd == "INTERSECTIONS") {
            Polygon target;
            if (std::cin >> target) {
                auto pred = std::bind(intersect, _1, std::cref(target));
                std::cout << std::count_if(shapes.begin(), shapes.end(), pred) << "\n";
            }
            else {
                std::cout << "<INVALID COMMAND>\n";
                skipUntilNewline();
            }
        }
        else {
            std::cout << "<INVALID COMMAND>\n";
            skipUntilNewline();
        }
    }
    return 0;
}
