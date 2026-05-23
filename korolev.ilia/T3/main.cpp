#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <iomanip>
#include <functional>
#include <limits>

struct Point {
    int x, y;
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};

struct Polygon {
    std::vector<Point> points;
    bool operator==(const Polygon& other) const {
        return points == other.points;
    }
};

int gaussTerm(const Point& a, const Point& b) {
    return a.x * b.y - b.x * a.y;
}

double getArea(const Polygon& poly) {
    if (poly.points.size() < 3) return 0.0;
    int sum = std::inner_product(poly.points.begin(), poly.points.end() - 1,
                                 poly.points.begin() + 1, 0,
                                 std::plus<>(), gaussTerm);
    sum += gaussTerm(poly.points.back(), poly.points.front());
    return std::abs(sum) / 2.0;
}

bool isEven(const Polygon& p) { return p.points.size() % 2 == 0; }
bool isOdd(const Polygon& p) { return p.points.size() % 2 != 0; }
bool hasNPoints(const Polygon& p, size_t n) { return p.points.size() == n; }

double addAreaEven(double acc, const Polygon& p) {
    return acc + (isEven(p) ? getArea(p) : 0.0);
}

double addAreaOdd(double acc, const Polygon& p) {
    return acc + (isOdd(p) ? getArea(p) : 0.0);
}

double addAreaAll(double acc, const Polygon& p) {
    return acc + getArea(p);
}

double addAreaN(double acc, const Polygon& p, size_t n) {
    return acc + (hasNPoints(p, n) ? getArea(p) : 0.0);
}

bool compArea(const Polygon& a, const Polygon& b) {
    return getArea(a) < getArea(b);
}

bool compVertex(const Polygon& a, const Polygon& b) {
    return a.points.size() < b.points.size();
}

bool isConsecutiveTarget(const Polygon& a, const Polygon& b, const Polygon& target) {
    return a == target && b == target;
}

Point subPoints(Point a, Point b) {
    return {a.x - b.x, a.y - b.y};
}

void normalizePolygon(Polygon& p) {
    Point base = p.points.front();
    std::transform(p.points.begin(), p.points.end(), p.points.begin(),
                   std::bind(subPoints, std::placeholders::_1, base));
}

bool isSame(const Polygon& p, const Polygon& target) {
    if (p.points.size() != target.points.size()) return false;
    Polygon normTarget = target;
    normalizePolygon(normTarget);
    size_t shift = 0;
    size_t n = p.points.size();
    while (shift < n) {
        Polygon shifted = p;
        std::rotate(shifted.points.begin(),
                    shifted.points.begin() + shift,
                    shifted.points.end());
        normalizePolygon(shifted);
        if (shifted == normTarget) return true;
        Polygon backward = p;
        std::reverse(backward.points.begin(), backward.points.end());
        std::rotate(backward.points.begin(),
                    backward.points.begin() + shift,
                    backward.points.end());
        normalizePolygon(backward);
        if (backward == normTarget) return true;
        shift++;
    }
    return false;
}

std::istream& operator>>(std::istream& is, Point& p) {
    char c1, c2, c3;
    if (is >> c1 && c1 == '(' &&
        is >> p.x >> c2 && c2 == ';' &&
        is >> p.y >> c3 && c3 == ')') {
        return is;
    }
    is.setstate(std::ios::failbit);
    return is;
}

bool Garbage() {
    char c;
    while (std::cin.get(c) && (c == ' ' || c == '\t' || c == '\r')) {}
    if (c != '\n' && c != EOF) {
        std::cin.putback(c);
        return true;
    }
    return false;
}

void skipInvalid() {
    std::cout << "<INVALID COMMAND>\n";
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int main(int argc, char* argv[]) {
    if (argc < 2) return 1;
    std::ifstream file(argv[1]);
    if (!file.is_open()) return 1;
    std::vector<Polygon> polygons;
    while (!file.eof()) {
        size_t num;
        if (file >> num) {
            if (num < 3) {
                file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }
            Polygon poly;
            size_t count = 0;
            Point p;
            bool valid = true;
            while (count < num) {
                if (!(file >> p)) { valid = false; break; }
                poly.points.push_back(p);
                count++;
            }
            if (valid) polygons.push_back(poly);
            else {
                file.clear();
                file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
        } else if (!file.eof()) {
            file.clear();
            file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
    file.close();
    std::cout << std::fixed << std::setprecision(1);
    std::string cmd;
    while (std::cin >> cmd) {
        if (cmd == "AREA") {
            std::string arg;
            if (!(std::cin >> arg)) continue;
            if (arg == "EVEN") {
                std::cout << std::accumulate(polygons.begin(),
                                             polygons.end(),
                                             0.0, addAreaEven) << "\n";
            } else if (arg == "ODD") {
                std::cout << std::accumulate(polygons.begin(),
                                             polygons.end(),
                                             0.0, addAreaOdd) << "\n";
            } else if (arg == "MEAN") {
                if (polygons.empty()) skipInvalid();
                else {
                    double total = std::accumulate(polygons.begin(),
                                                   polygons.end(),
                                                   0.0, addAreaAll);
                    std::cout << total / polygons.size() << "\n";
                }
            } else {
                auto isDigit = [](unsigned char c) { return std::isdigit(c); };
                if (std::all_of(arg.begin(), arg.end(), isDigit)) {
                    size_t num = std::stoi(arg);
                    if (num < 3) skipInvalid();
                    else {
                        auto bindAreaN = std::bind(addAreaN,
                                                   std::placeholders::_1,
                                                   std::placeholders::_2,
                                                   num);
                        std::cout << std::accumulate(polygons.begin(),
                                                     polygons.end(),
                                                     0.0, bindAreaN) << "\n";
                    }
                } else skipInvalid();
            }
        }
        else if (cmd == "MAX" || cmd == "MIN") {
            std::string arg;
            if (std::cin >> arg && !polygons.empty()) {
                if (arg == "AREA") {
                    auto it = (cmd == "MAX") ?
                        std::max_element(polygons.begin(),
                                         polygons.end(), compArea) :
                        std::min_element(polygons.begin(),
                                         polygons.end(), compArea);
                    std::cout << getArea(*it) << "\n";
                } else if (arg == "VERTEXES") {
                    auto it = (cmd == "MAX") ?
                        std::max_element(polygons.begin(),
                                         polygons.end(), compVertex) :
                        std::min_element(polygons.begin(),
                                         polygons.end(), compVertex);
                    std::cout << it->points.size() << "\n";
                } else skipInvalid();
            } else skipInvalid();
        }
        else if (cmd == "COUNT") {
            std::string arg;
            if (!(std::cin >> arg)) continue;
            if (arg == "EVEN") {
                std::cout << std::count_if(polygons.begin(),
                                           polygons.end(), isEven) << "\n";
            } else if (arg == "ODD") {
                std::cout << std::count_if(polygons.begin(),
                                           polygons.end(), isOdd) << "\n";
            } else {
                auto isDigit = [](unsigned char c) { return std::isdigit(c); };
                if (std::all_of(arg.begin(), arg.end(), isDigit)) {
                    size_t num = std::stoi(arg);
                    if (num < 3) skipInvalid();
                    else {
                        auto bindHasN = std::bind(hasNPoints,
                                                  std::placeholders::_1, num);
                        std::cout << std::count_if(polygons.begin(),
                                                   polygons.end(),
                                                   bindHasN) << "\n";
                    }
                } else skipInvalid();
            }
        }
        else if (cmd == "RMECHO") {
            size_t num;
            if (!(std::cin >> num) || num < 3) skipInvalid();
            else {
                Polygon target;
                bool valid = true;
                size_t count = 0;
                while (count < num) {
                    Point p;
                    if (!(std::cin >> p)) { valid = false; break; }
                    target.points.push_back(p);
                    count++;
                }
                if (valid && !Garbage()) {
                    size_t oldSize = polygons.size();
                    auto bindTarget = std::bind(isConsecutiveTarget,
                                                std::placeholders::_1,
                                                std::placeholders::_2,
                                                target);
                    auto it = std::unique(polygons.begin(),
                                          polygons.end(), bindTarget);
                    polygons.erase(it, polygons.end());
                    std::cout << (oldSize - polygons.size()) << "\n";
                } else skipInvalid();
            }
        }
        else if (cmd == "SAME") {
            size_t num;
            if (!(std::cin >> num) || num < 3) skipInvalid();
            else {
                Polygon target;
                bool valid = true;
                size_t count = 0;
                while (count < num) {
                    Point p;
                    if (!(std::cin >> p)) { valid = false; break; }
                    target.points.push_back(p);
                    count++;
                }
                if (valid && !Garbage()) {
                    auto bindSame = std::bind(isSame,
                                              std::placeholders::_1,
                                              target);
                    std::cout << std::count_if(polygons.begin(),
                                               polygons.end(),
                                               bindSame) << "\n";
                } else skipInvalid();
            }
        }
        else skipInvalid();
    }
    return 0;
}
