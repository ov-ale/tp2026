#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <iomanip>
#include <limits>

struct Point {
    int x, y;
};

struct Polygon {
    std::vector<Point> points;
};

double getArea(const Polygon& poly) {
    if (poly.points.size() < 3) return 0.0;
    size_t n = poly.points.size();
    std::vector<size_t> indices(n);
    std::iota(indices.begin(), indices.end(), 0);
    int doubleArea = std::accumulate(
        indices.begin(),
        indices.end(),
        0,
        [&](int acc, size_t i) {
            size_t next = (i + 1) % n;
            return acc + (poly.points[i].x * poly.points[next].y -
                          poly.points[next].x * poly.points[i].y);
        }
    );
    return std::abs(doubleArea) / 2.0;
}

bool areIdentical(const Polygon& a, const Polygon& b) {
    if (a.points.size() != b.points.size()) return false;
    return std::equal(
        a.points.begin(),
        a.points.end(),
        b.points.begin(),
        [](const Point& p1, const Point& p2) {
            return p1.x == p2.x && p1.y == p2.y;
        }
    );
}

bool isSame(const Polygon& a, const Polygon& b) {
    if (a.points.size() != b.points.size()) return false;
    size_t n = a.points.size();
    std::vector<size_t> indices(n);
    std::iota(indices.begin(), indices.end(), 0);
    return std::any_of(
        indices.begin(),
        indices.end(),
        [&](size_t i) {
            int dx = b.points[0].x - a.points[i].x;
            int dy = b.points[0].y - a.points[i].y;
            bool forward = std::all_of(
                indices.begin(),
                indices.end(),
                [&](size_t j) {
                    size_t a_idx = (i + j) % n;
                    return (b.points[j].x == a.points[a_idx].x + dx) &&
                           (b.points[j].y == a.points[a_idx].y + dy);
                }
            );
            if (forward) return true;
            return std::all_of(
                indices.begin(),
                indices.end(),
                [&](size_t j) {
                    size_t a_idx = (i + n - j) % n;
                    return (b.points[j].x == a.points[a_idx].x + dx) &&
                           (b.points[j].y == a.points[a_idx].y + dy);
                }
            );
        }
    );
}

bool readPolygon(std::istream& in, Polygon& poly) {
    size_t num;
    if (!(in >> num) || num < 3) return false;
    poly.points.resize(num);
    bool success = std::all_of(poly.points.begin(), poly.points.end(), [&](Point& p) {
        char c1, c2, c3;
        return (in >> c1 && c1 == '(' && in >> p.x >> c2
            && c2 == ';' && in >> p.y >> c3 && c3 == ')');
    });
    if (!success) return false;
    if (in >> std::ws && in.peek() == '(') {
        return false;
    }
    return true;
}

bool isNumber(const std::string& s) {
    return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}

void skipInvalid() {
    std::cout << "<INVALID COMMAND>\n";
    std::cin.clear();
    std::cin.ignore(
        std::numeric_limits<std::streamsize>::max(),
        '\n'
    );
}

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
                char c1, c2, c3;
                if (!(file >> c1 && c1 == '(' &&
                      file >> poly.points[i].x >> c2 &&
                      c2 == ';' &&
                      file >> poly.points[i].y >> c3 &&
                      c3 == ')')) {
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
            if (arg == "EVEN") {
                double res = std::accumulate(
                    polygons.begin(),
                    polygons.end(),
                    0.0,
                    [](double acc, const Polygon& p) {
                        return p.points.size() % 2 == 0 ?
                               acc + getArea(p) : acc;
                    }
                );
                std::cout << res << "\n";
            } else if (arg == "ODD") {
                double res = std::accumulate(
                    polygons.begin(),
                    polygons.end(),
                    0.0,
                    [](double acc, const Polygon& p) {
                        return p.points.size() % 2 != 0 ?
                               acc + getArea(p) : acc;
                    }
                );
                std::cout << res << "\n";
            } else if (arg == "MEAN") {
                if (polygons.empty()) {
                    skipInvalid();
                } else {
                    double sum = std::accumulate(
                        polygons.begin(),
                        polygons.end(),
                        0.0,
                        [](double acc, const Polygon& p) {
                            return acc + getArea(p);
                        }
                    );
                    std::cout << sum / polygons.size() << "\n";
                }
            } else if (isNumber(arg)) {
                size_t num = std::stoi(arg);
                if (num < 3) {
                    skipInvalid();
                    continue;
                }
                double res = std::accumulate(
                    polygons.begin(),
                    polygons.end(),
                    0.0,
                    [num](double acc, const Polygon& p) {
                        return p.points.size() == num ?
                               acc + getArea(p) : acc;
                    }
                );
                std::cout << res << "\n";
            } else {
                skipInvalid();
            }
        } else if (cmd == "MAX") {
            std::string arg;
            if (!(std::cin >> arg)) continue;

            if (arg == "AREA") {
                if (polygons.empty()) skipInvalid();
                else {
                    auto it = std::max_element(
                        polygons.begin(),
                        polygons.end(),
                        [](const Polygon& a, const Polygon& b) {
                            return getArea(a) < getArea(b);
                        }
                    );
                    std::cout << getArea(*it) << "\n";
                }
            } else if (arg == "VERTEXES") {
                if (polygons.empty()) skipInvalid();
                else {
                    auto it = std::max_element(
                        polygons.begin(),
                        polygons.end(),
                        [](const Polygon& a, const Polygon& b) {
                            return a.points.size() < b.points.size();
                        }
                    );
                    std::cout << it->points.size() << "\n";
                }
            } else {
                skipInvalid();
            }
        } else if (cmd == "MIN") {
            std::string arg;
            if (!(std::cin >> arg)) continue;

            if (arg == "AREA") {
                if (polygons.empty()) skipInvalid();
                else {
                    auto it = std::min_element(
                        polygons.begin(),
                        polygons.end(),
                        [](const Polygon& a, const Polygon& b) {
                            return getArea(a) < getArea(b);
                        }
                    );
                    std::cout << getArea(*it) << "\n";
                }
            } else if (arg == "VERTEXES") {
                if (polygons.empty()) skipInvalid();
                else {
                    auto it = std::min_element(
                        polygons.begin(),
                        polygons.end(),
                        [](const Polygon& a, const Polygon& b) {
                            return a.points.size() < b.points.size();
                        }
                    );
                    std::cout << it->points.size() << "\n";
                }
            } else {
                skipInvalid();
            }
        } else if (cmd == "COUNT") {
            std::string arg;
            if (!(std::cin >> arg)) continue;
            if (arg == "EVEN") {
                long long cnt = std::count_if(
                    polygons.begin(),
                    polygons.end(),
                    [](const Polygon& p) {
                        return p.points.size() % 2 == 0;
                    }
                );
                std::cout << cnt << "\n";
            } else if (arg == "ODD") {
                long long cnt = std::count_if(
                    polygons.begin(),
                    polygons.end(),
                    [](const Polygon& p) {
                        return p.points.size() % 2 != 0;
                    }
                );
                std::cout << cnt << "\n";
            } else if (isNumber(arg)) {
                size_t num = std::stoi(arg);
                if (num < 3) {
                    skipInvalid();
                    continue;
                }
                long long cnt = std::count_if(
                    polygons.begin(),
                    polygons.end(),
                    [num](const Polygon& p) {
                        return p.points.size() == num;
                    }
                );
                std::cout << cnt << "\n";
            } else {
                skipInvalid();
            }
        } else if (cmd == "RMECHO") {
            Polygon target;
            if (readPolygon(std::cin, target)) {
                auto it = std::unique(
                    polygons.begin(),
                    polygons.end(),
                    [&](const Polygon& a, const Polygon& b) {
                        return areIdentical(a, target) &&
                               areIdentical(b, target);
                    }
                );
                auto removedCount = std::distance(it, polygons.end());
                polygons.erase(it, polygons.end());
                std::cout << removedCount << "\n";
            } else {
                skipInvalid();
            }
        } else if (cmd == "SAME") {
            Polygon target;
            if (readPolygon(std::cin, target)) {
                long long cnt = std::count_if(
                    polygons.begin(),
                    polygons.end(),
                    [&](const Polygon& p) {
                        return isSame(p, target);
                    }
                );
                std::cout << cnt << "\n";
            } else {
                skipInvalid();
            }
        } else {
            skipInvalid();
        }
    }
    return 0;
}
