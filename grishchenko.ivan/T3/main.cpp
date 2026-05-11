#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <limits>
struct Point {
    int x, y;
};
struct Polygon {
    std::vector<Point> points;
};
double getArea(const Polygon& p) {
    double area = 0.0;
    size_t n = p.points.size();
    if (n < 3) return 0.0;
    for (size_t i = 0; i < n; ++i) {
        area += (1.0 * p.points[i].x * p.points[(i + 1) % n].y);
        area -= (1.0 * p.points[(i + 1) % n].x * p.points[i].y);
    }
    return std::abs(area) / 2.0;
}
bool areEqual(const Polygon& a, const Polygon& b) {
    if (a.points.size() != b.points.size()) return false;
    return std::equal(a.points.begin(), a.points.end(), b.points.begin(),
        [](const Point& p1, const Point& p2) {
            return p1.x == p2.x && p1.y == p2.y;
        });
}
std::istream& consume(std::istream& is, char expected) {
    char ch;
    if (!(is >> ch)) return is;
    if (ch != expected) is.setstate(std::ios::failbit);
    return is;
}
std::istream& operator>>(std::istream& is, Point& p) {
    std::istream::sentry sentry(is);
    if (!sentry) return is;
    if (!consume(is, '(')) return is;
    if (!(is >> p.x)) return is;
    if (!consume(is, ';')) return is;
    if (!(is >> p.y)) return is;
    if (!consume(is, ')')) return is;
    return is;
}
std::istream& operator>>(std::istream& is, Polygon& poly) {
    std::istream::sentry sentry(is);
    if (!sentry) return is;
    size_t n;
    if (!(is >> n)) return is;
    std::vector<Point> tmp;
    for (size_t i = 0; i < n; ++i) {
        Point p;
        if (!(is >> p)) return is;
        tmp.push_back(p);
    }
    if (n < 3) {
        is.setstate(std::ios::failbit);
        return is;
    }
    poly.points = std::move(tmp);
    return is;
}
void readTarget(Polygon& target) {
    std::string line;
    std::getline(std::cin >> std::ws, line);
    std::stringstream ss(line);
    if (!(ss >> target)) throw std::logic_error("");
    std::string extra;
    if (ss >> extra) throw std::logic_error("");
}
void cmdArea(const std::vector<Polygon>& shapes) {
    std::string arg;
    std::cin >> arg;
    double res = 0;
    if (arg == "EVEN") {
        res = std::accumulate(shapes.begin(), shapes.end(), 0.0, [](double t, const Polygon& p) {
            return (p.points.size() % 2 == 0) ? t + getArea(p) : t;
        });
    } else if (arg == "ODD") {
        res = std::accumulate(shapes.begin(), shapes.end(), 0.0, [](double t, const Polygon& p) {
            return (p.points.size() % 2 != 0) ? t + getArea(p) : t;
        });
    } else if (arg == "MEAN") {
        if (shapes.empty()) throw std::logic_error("");
        res = std::accumulate(shapes.begin(), shapes.end(), 0.0, [](double t, const Polygon& p) {
            return t + getArea(p);
        }) / shapes.size();
    } else if (std::isdigit(arg[0])) {
        size_t n = std::stoul(arg);
        res = std::accumulate(shapes.begin(), shapes.end(), 0.0, [n](double t, const Polygon& p) {
            return (p.points.size() == n) ? t + getArea(p) : t;
        });
    } else throw std::logic_error("");
    std::cout << std::fixed << std::setprecision(1) << res << "\n";
}
void cmdMax(const std::vector<Polygon>& shapes) {
    if (shapes.empty()) throw std::logic_error("");
    std::string arg;
    std::cin >> arg;
    if (arg == "AREA") {
        auto it = std::max_element(shapes.begin(), shapes.end(), [](const Polygon& a, const Polygon& b) {
            return getArea(a) < getArea(b);
        });
        std::cout << std::fixed << std::setprecision(1) << getArea(*it) << "\n";
    } else if (arg == "VERTEXES") {
        auto it = std::max_element(shapes.begin(), shapes.end(), [](const Polygon& a, const Polygon& b) {
            return a.points.size() < b.points.size();
        });
        std::cout << it->points.size() << "\n";
    } else throw std::logic_error("");
}
void cmdMin(const std::vector<Polygon>& shapes) {
    if (shapes.empty()) throw std::logic_error("");
    std::string arg;
    std::cin >> arg;
    if (arg == "AREA") {
        auto it = std::min_element(shapes.begin(), shapes.end(), [](const Polygon& a, const Polygon& b) {
            return getArea(a) < getArea(b);
        });
        std::cout << std::fixed << std::setprecision(1) << getArea(*it) << "\n";
    } else if (arg == "VERTEXES") {
        auto it = std::min_element(shapes.begin(), shapes.end(), [](const Polygon& a, const Polygon& b) {
            return a.points.size() < b.points.size();
        });
        std::cout << it->points.size() << "\n";
    } else throw std::logic_error("");
}
void cmdCount(const std::vector<Polygon>& shapes) {
    std::string arg;
    std::cin >> arg;
    size_t res = 0;
    if (arg == "EVEN") {
        res = std::count_if(shapes.begin(), shapes.end(), [](const Polygon& p) { return p.points.size() % 2 == 0; });
    } else if (arg == "ODD") {
        res = std::count_if(shapes.begin(), shapes.end(), [](const Polygon& p) { return p.points.size() % 2 != 0; });
    } else if (std::isdigit(arg[0])) {
        size_t n = std::stoul(arg);
        res = std::count_if(shapes.begin(), shapes.end(), [n](const Polygon& p) { return p.points.size() == n; });
    } else throw std::logic_error("");
    std::cout << res << "\n";
}
void cmdEcho(std::vector<Polygon>& shapes) {
    Polygon target;
    readTarget(target);
    std::vector<Polygon> result;
    size_t added = 0;
    for (const auto& p : shapes) {
        result.push_back(p);
        if (areEqual(p, target)) {
            result.push_back(p);
            added++;
        }
    }
    shapes = std::move(result);
    std::cout << added << "\n";
}
void cmdMaxSeq(const std::vector<Polygon>& shapes) {
    Polygon target;
    readTarget(target);
    int max_run = 0, current_run = 0;
    for (const auto& p : shapes) {
        if (areEqual(p, target)) {
            current_run++;
            max_run = std::max(max_run, current_run);
        } else {
            current_run = 0;
        }
    }
    std::cout << max_run << "\n";
}
int main(int argc, char* argv[]) {
    if (argc < 2) return 1;
    std::ifstream file(argv[1]);
    if (!file) return 1;
    std::vector<Polygon> shapes;
    while (!file.eof()) {
        Polygon p;
        if (file >> p) shapes.push_back(p);
        else {
            file.clear();
            file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
    std::string cmd;
    while (std::cin >> cmd) {
        try {
            if (cmd == "AREA") cmdArea(shapes);
            else if (cmd == "MAX") cmdMax(shapes);
            else if (cmd == "MIN") cmdMin(shapes);
            else if (cmd == "COUNT") cmdCount(shapes);
            else if (cmd == "ECHO") cmdEcho(shapes);
            else if (cmd == "MAXSEQ") cmdMaxSeq(shapes);
            else {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                throw std::logic_error("");
            }
        } catch (...) {
            std::cout << "<INVALID COMMAND>\n";
            std::cin.clear();
            if (std::cin.eof()) break;
        }
    }
    return 0;
}
