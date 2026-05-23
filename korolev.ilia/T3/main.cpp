#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <iomanip>
#include <limits>

struct Point { int x, y; };
struct Polygon { std::vector<Point> points; };

bool comparePoints(const Point& p1, const Point& p2) {
    return p1.x == p2.x && p1.y == p2.y;
}

int addAreaTerm(int acc, size_t i, const Polygon& poly) {
    size_t next = (i + 1) % poly.points.size();
    return acc + (poly.points[i].x * poly.points[next].y -
                  poly.points[next].x * poly.points[i].y);
}

double getArea(const Polygon& poly) {
    if (poly.points.size() < 3) return 0.0;
    int doubleArea = 0;
    for (size_t i = 0; i < poly.points.size(); ++i) {
        doubleArea = addAreaTerm(doubleArea, i, poly);
    }
    return std::abs(doubleArea) / 2.0;
}

bool areIdentical(const Polygon& a, const Polygon& b) {
    if (a.points.size() != b.points.size()) return false;
    return std::equal(a.points.begin(), a.points.end(),
                      b.points.begin(), comparePoints);
}

bool checkShift(const Polygon& a, const Polygon& b, size_t i, bool forward) {
    size_t n = a.points.size();
    int dx = b.points[0].x - a.points[i].x;
    int dy = b.points[0].y - a.points[i].y;
    for (size_t j = 0; j < n; ++j) {
        size_t a_idx = forward ? (i + j) % n : (i + n - j) % n;
        if ((b.points[j].x != a.points[a_idx].x + dx) ||
            (b.points[j].y != a.points[a_idx].y + dy)) return false;
    }
    return true;
}

bool isSame(const Polygon& a, const Polygon& b) {
    if (a.points.size() != b.points.size()) return false;
    for (size_t i = 0; i < a.points.size(); ++i) {
        if (checkShift(a, b, i, true) || checkShift(a, b, i, false))
            return true;
    }
    return false;
}

bool readSinglePoint(std::istream& in, Point& p) {
    char c1, c2, c3;
    return (in >> c1 && c1 == '(' && in >> p.x >> c2 &&
            c2 == ';' && in >> p.y >> c3 && c3 == ')');
}

bool readPolygon(std::istream& in, Polygon& poly) {
    size_t num;
    if (!(in >> num) || num < 3) return false;
    poly.points.resize(num);
    for (size_t i = 0; i < num; ++i) {
        if (!readSinglePoint(in, poly.points[i])) return false;
    }
    char next;
    while (in.get(next)) {
        if (next == '\n') return true;
        if (next != ' ' && next != '\t') return false;
    }
    return true;
}

bool isNumber(const std::string& s) {
    if (s.empty()) return false;
    for (char const &c : s) if (!std::isdigit(c)) return false;
    return true;
}

void skipInvalid(std::istream& in) {
    std::cout << "<INVALID COMMAND>\n";
    in.clear();
    in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

bool isEvenSize(const Polygon& p) { return p.points.size() % 2 == 0; }
bool isOddSize(const Polygon& p) { return p.points.size() % 2 != 0; }

int main(int argc, char* argv[]) {
    if (argc < 2) return 1;
    std::ifstream file(argv[1]);
    if (!file.is_open()) return 1;

    std::vector<Polygon> polygons;
    while (!file.eof()) {
        size_t num;
        if (file >> num) {
            if (num < 3) continue;
            Polygon poly;
            poly.points.resize(num);
            bool valid = true;
            for (size_t i = 0; i < num; ++i) {
                if (!readSinglePoint(file, poly.points[i])) {
                    valid = false;
                    file.clear();
                    break;
                }
            }
            if (valid) polygons.push_back(poly);
        } else {
            file.clear();
            char dummy;
            if (!(file >> dummy)) break;
        }
    }
    file.close();

    std::cout << std::fixed << std::setprecision(1);
    std::string cmd;

    while (std::cin >> cmd) {
        if (cmd == "AREA") {
            std::string arg;
            if (!(std::cin >> arg)) continue;
            if (arg == "EVEN" || arg == "ODD") {
                double res = 0.0;
                for (const auto& p : polygons) {
                    if ((arg == "EVEN" && isEvenSize(p)) ||
                        (arg == "ODD" && isOddSize(p))) {
                        res += getArea(p);
                    }
                }
                std::cout << res << "\n";
            } else if (arg == "MEAN") {
                if (polygons.empty()) skipInvalid(std::cin);
                else {
                    double total = 0.0;
                    for (const auto& p : polygons) total += getArea(p);
                    std::cout << total / polygons.size() << "\n";
                }
            } else if (isNumber(arg)) {
                size_t num = std::stoi(arg);
                if (num < 3) skipInvalid(std::cin);
                else {
                    double res = 0.0;
                    for (const auto& p : polygons) {
                        if (p.points.size() == num) res += getArea(p);
                    }
                    std::cout << res << "\n";
                }
            } else skipInvalid(std::cin);
        } else if (cmd == "MAX" || cmd == "MIN") {
            std::string arg;
            if (!(std::cin >> arg)) continue;
            if (arg == "AREA" || arg == "VERTEXES") {
                if (polygons.empty()) skipInvalid(std::cin);
                else {
                    auto comp = [&arg](const Polygon& a, const Polygon& b) {
                        if (arg == "AREA") return getArea(a) < getArea(b);
                        return a.points.size() < b.points.size();
                    };
                    auto it = (cmd == "MAX") ?
                        std::max_element(polygons.begin(), polygons.end(), comp) :
                        std::min_element(polygons.begin(), polygons.end(), comp);
                    if (arg == "AREA") std::cout << getArea(*it) << "\n";
                    else std::cout << it->points.size() << "\n";
                }
            } else skipInvalid(std::cin);
        } else if (cmd == "COUNT") {
            std::string arg;
            if (!(std::cin >> arg)) continue;
            if (arg == "EVEN") {
                std::cout << std::count_if(polygons.begin(),
                    polygons.end(), isEvenSize) << "\n";
            } else if (arg == "ODD") {
                std::cout << std::count_if(polygons.begin(),
                    polygons.end(), isOddSize) << "\n";
            } else if (isNumber(arg)) {
                size_t num = std::stoi(arg);
                if (num < 3) skipInvalid(std::cin);
                else {
                    long long cnt = 0;
                    for (const auto& p : polygons) {
                        if (p.points.size() == num) cnt++;
                    }
                    std::cout << cnt << "\n";
                }
            } else skipInvalid(std::cin);
        } else if (cmd == "RMECHO") {
            Polygon target;
            if (readPolygon(std::cin, target)) {
                long long removed = 0;
                std::vector<Polygon> updated;
                for (const auto& p : polygons) {
                    if (areIdentical(p, target)) removed++;
                    else updated.push_back(p);
                }
                polygons = updated;
                std::cout << removed << "\n";
            } else skipInvalid(std::cin);
        } else if (cmd == "SAME") {
            Polygon target;
            if (readPolygon(std::cin, target)) {
                long long cnt = 0;
                for (const auto& p : polygons) {
                    if (isSame(p, target)) cnt++;
                }
                std::cout << cnt << "\n";
            } else skipInvalid(std::cin);
        } else skipInvalid(std::cin);
    }
    return 0;
}
