#include <iostream>
#include <vector>
#include <iterator>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <functional>
#include <numeric>
#include <iomanip>
#include <limits>
#include <cctype>
#include <cmath>

struct Point {
    int x, y;
};

bool operator==(const Point& a, const Point& b) {
    return a.x == b.x && a.y == b.y;
}

struct Polygon {
    std::vector<Point> points;
};

std::istream& operator>>(std::istream& in, Point& point) {
    char leftBr = 0;
    char semicolon = 0;
    char rightBr = 0;
    int x = 0;
    int y = 0;

    if (in >> leftBr >> x >> semicolon >> y >> rightBr) {
        if (leftBr == '(' && semicolon == ';' && rightBr == ')') {
            point.x = x;
            point.y = y;
        } else {
            in.setstate(std::ios::failbit);
        }
    }
    return in;
}

std::istream& operator>>(std::istream& in, Polygon& poly) {
    size_t n = 0;
    if (!(in >> n)) {
        in.setstate(std::ios::failbit);
        return in;
    }

    if (n < 3) {
        in.setstate(std::ios::failbit);
        return in;
    }

    std::vector<Point> points;
    points.reserve(n);

    std::copy_n(
        std::istream_iterator<Point>(in),
        n,
        std::back_inserter(points)
    );

    if (!in || points.size() != n) {
        in.setstate(std::ios::failbit);
        return in;
    }
    poly.points = std::move(points);
    return in;
}

std::vector<Polygon> readPolygons(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: cannot open file: " << filename << std::endl;
        return {};
    }

    std::vector<Polygon> result;
    std::string line;

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        while (!line.empty() && std::isspace(static_cast<unsigned char>(line.back()))) {
            line.pop_back();
        }

        if (line.empty()) continue;

        std::istringstream iss(line);

        Polygon p;
        if (iss >> p) {
            char remaining;
            if (!(iss >> remaining)) {
                result.push_back(p);
            }
        }
    }

    return result;
}

struct CrossProduct {
    long long operator()(const Point& a, const Point& b) const {
        return static_cast<long long>(a.x) * b.y -
               static_cast<long long>(b.x) * a.y;
    }
};

double computeArea(const Polygon& poly) {
    const auto& pts = poly.points;
    if (pts.size() < 3) return 0.0;

    long long sum = std::inner_product(
        pts.begin(), pts.end() - 1,
        pts.begin() + 1,
        0LL,
        std::plus<long long>(),
        CrossProduct()
    );
    sum += CrossProduct()(pts.back(), pts.front());

    return std::abs(sum) / 2.0;
}

struct PolygonEqual {
    const Polygon& target;
    explicit PolygonEqual(const Polygon& t) : target(t) {}

    bool operator()(const Polygon& p) const {
        if (p.points.size() != target.points.size()) return false;
        return std::equal(p.points.begin(), p.points.end(), target.points.begin());
    }
};

struct AreaSum {
    double operator()(double sum, const Polygon& p) const {
        return sum + computeArea(p);
    }
};

struct AreaEven {
    double operator()(double sum, const Polygon& p) const {
        return sum + (p.points.size() % 2 == 0 ? computeArea(p) : 0.0);
    }
};

struct AreaOdd {
    double operator()(double sum, const Polygon& p) const {
        return sum + (p.points.size() % 2 == 1 ? computeArea(p) : 0.0);
    }
};

struct AreaWithVertexCount {
    size_t target;
    explicit AreaWithVertexCount(size_t t) : target(t) {}

    double operator()(double sum, const Polygon& p) const {
        return sum + (p.points.size() == target ? computeArea(p) : 0.0);
    }
};

struct AreaGreater {
    bool operator()(const Polygon& a, const Polygon& b) const {
        return computeArea(a) < computeArea(b);
    }
};

struct VertexCountGreater {
    bool operator()(const Polygon& a, const Polygon& b) const {
        return a.points.size() < b.points.size();
    }
};

struct CountEven {
    bool operator()(const Polygon& p) const {
        return p.points.size() % 2 == 0;
    }
};

struct CountOdd {
    bool operator()(const Polygon& p) const {
        return p.points.size() % 2 == 1;
    }
};

struct CountWithVertexCount {
    size_t target;
    explicit CountWithVertexCount(size_t t) : target(t) {}

    bool operator()(const Polygon& p) const {
        return p.points.size() == target;
    }
};

struct PointXLess {
    bool operator()(const Point& a, const Point& b) const {
        return a.x < b.x;
    }
};

struct PointXGreater {
    bool operator()(const Point& a, const Point& b) const {
        return a.x > b.x;
    }
};

struct PointYLess {
    bool operator()(const Point& a, const Point& b) const {
        return a.y < b.y;
    }
};

struct PointYGreater {
    bool operator()(const Point& a, const Point& b) const {
        return a.y > b.y;
    }
};

struct PointInFrame {
    int min_x, max_x, min_y, max_y;

    PointInFrame(int minx, int maxx, int miny, int maxy)
        : min_x(minx), max_x(maxx), min_y(miny), max_y(maxy) {}

    bool operator()(const Point& p) const {
        return p.x >= min_x && p.x <= max_x && p.y >= min_y && p.y <= max_y;
    }
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Error: file name missing\n";
        return 1;
    }

    std::string filename = argv[1];
    std::vector<Polygon> polygons = readPolygons(filename);

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;

        std::istringstream iss(line);
        std::string command;
        if (!(iss >> command)) continue;

        if (command == "AREA") {
            std::string param;
            if (!(iss >> param)) {
                std::cout << "<INVALID COMMAND>\n";
                continue;
            }

            char remaining;
            if (iss >> remaining) {
                std::cout << "<INVALID COMMAND>\n";
                continue;
            }

            if (param == "MEAN") {
                if (polygons.empty()) {
                    std::cout << "<INVALID COMMAND>\n";
                     continue;
                }
                double sum = std::accumulate(polygons.begin(), polygons.end(), 0.0, AreaSum());
                std::cout << std::fixed << std::setprecision(1) << sum / polygons.size() << "\n";
            }
            else if (param == "EVEN") {
                double sum = std::accumulate(polygons.begin(), polygons.end(), 0.0, AreaEven());
                std::cout << std::fixed << std::setprecision(1) << sum << "\n";
            }
            else if (param == "ODD") {
                double sum = std::accumulate(polygons.begin(), polygons.end(), 0.0, AreaOdd());
                std::cout << std::fixed << std::setprecision(1) << sum << "\n";
            }
            else {
                try {
                    size_t vertexCount = std::stoul(param);
                    if (vertexCount < 3) {
                        std::cout << "<INVALID COMMAND>\n";
                    } else {
                        double sum = std::accumulate(polygons.begin(), polygons.end(), 0.0,
                        AreaWithVertexCount(vertexCount));
                        std::cout << std::fixed << std::setprecision(1) << sum << "\n";
                    }
                } catch (...) {
                    std::cout << "<INVALID COMMAND>\n";
                }
            }
        }
        else if (command == "MAX") {
            std::string param;
            if (!(iss >> param) || polygons.empty()) {
                std::cout << "<INVALID COMMAND>\n";
                continue;
            }

            char remaining;
            if (iss >> remaining) {
                std::cout << "<INVALID COMMAND>\n";
                continue;
            }

            if (param == "AREA") {
                auto it = std::max_element(polygons.begin(), polygons.end(), AreaGreater());
                std::cout << std::fixed << std::setprecision(1) << computeArea(*it) << "\n";
            }
            else if (param == "VERTEXES") {
                auto it = std::max_element(polygons.begin(), polygons.end(), VertexCountGreater());
                std::cout << it->points.size() << "\n";
            }
            else {
                std::cout << "<INVALID COMMAND>\n";
            }
        }
        else if (command == "MIN") {
            std::string param;
            if (!(iss >> param) || polygons.empty()) {
                std::cout << "<INVALID COMMAND>\n";
                continue;
            }

            char remaining;
            if (iss >> remaining) {
                std::cout << "<INVALID COMMAND>\n";
                continue;
            }

            if (param == "AREA") {
                auto it = std::min_element(polygons.begin(), polygons.end(), AreaGreater());
                std::cout << std::fixed << std::setprecision(1) << computeArea(*it) << "\n";
            }
            else if (param == "VERTEXES") {
                auto it = std::min_element(polygons.begin(), polygons.end(), VertexCountGreater());
                std::cout << it->points.size() << "\n";
            }
            else {
                std::cout << "<INVALID COMMAND>\n";
            }
        }
        else if (command == "COUNT") {
            std::string param;
            if (!(iss >> param)) {
                std::cout << "<INVALID COMMAND>\n";
                continue;
            }

            char remaining;
            if (iss >> remaining) {
                std::cout << "<INVALID COMMAND>\n";
                continue;
            }

            if (param == "EVEN") {
                size_t count = std::count_if(polygons.begin(), polygons.end(), CountEven());
                std::cout << count << "\n";
            }
            else if (param == "ODD") {
                size_t count = std::count_if(polygons.begin(), polygons.end(), CountOdd());
                std::cout << count << "\n";
            }
            else {
                try {
                    size_t vertexCount = std::stoul(param);
                    if (vertexCount < 3) {
                        std::cout << "<INVALID COMMAND>\n";
                    } else {
                        size_t count = std::count_if(polygons.begin(), polygons.end(),
                            CountWithVertexCount(vertexCount));
                        std::cout << count << "\n";
                    }
                } catch (...) {
                    std::cout << "<INVALID COMMAND>\n";
                }
            }
        }
        else if (command == "ECHO") {
            Polygon target;
            if (!(iss >> target)) {
                std::cout << "<INVALID COMMAND>\n";
                continue;
            }

            char remaining;
            if (iss >> remaining) {
                std::cout << "<INVALID COMMAND>\n";
                continue;
            }

            int added = 0;
            PolygonEqual equal(target);

            for (size_t i = 0; i < polygons.size(); ++i) {
                if (equal(polygons[i])) {
                    polygons.insert(polygons.begin() + i + 1, target);
                    added++;
                    i++;
                }
            }
            std::cout << added << "\n";
        }
        else if (command == "INFRAME") {
            int expectedCount;
            if (!(iss >> expectedCount)) {
                std::cout << "<INVALID COMMAND>\n";
                continue;
            }

            std::vector<Point> points;
            bool invalidInput = false;

            for (int i = 0; i < expectedCount; ++i) {
                char openBracket, semiColon, closeBracket;
                int x, y;

                if (!(iss >> openBracket >> x >> semiColon >> y >> closeBracket) ||
                    openBracket != '(' || semiColon != ';' || closeBracket != ')') {
                    invalidInput = true;
                    break;
                }
                points.push_back({x, y});
            }

            char remaining;
            if (iss >> remaining) {
                invalidInput = true;
            }

            if (invalidInput || points.size() != static_cast<size_t>(expectedCount) || expectedCount < 3) {
                std::cout << "<INVALID COMMAND>\n";
            }
            else if (polygons.empty()) {
                std::cout << "<INVALID COMMAND>\n";
            }
            else {
                Polygon target;
                target.points = points;

                int min_x = polygons[0].points[0].x;
                int max_x = polygons[0].points[0].x;
                int min_y = polygons[0].points[0].y;
                int max_y = polygons[0].points[0].y;

                for (const auto& poly : polygons) {
                    for (const auto& p : poly.points) {
                        if (p.x < min_x) min_x = p.x;
                        if (p.x > max_x) max_x = p.x;
                        if (p.y < min_y) min_y = p.y;
                        if (p.y > max_y) max_y = p.y;
                    }
                }

                bool all_inside = true;
                for (const auto& p : target.points) {
                    if (p.x < min_x || p.x > max_x || p.y < min_y || p.y > max_y) {
                        all_inside = false;
                        break;
                    }
                }
                std::cout << (all_inside ? "<TRUE>" : "<FALSE>") << "\n";
            }
        }
        else {
            std::cout << "<INVALID COMMAND>\n";
        }
    }
    return 0;
}
