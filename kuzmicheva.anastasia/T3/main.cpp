#include <vector>
#include <iostream>
#include <fstream>
#include <numeric>
#include <cstdlib>
#include <string>
#include <cmath>
#include <limits>
#include <iomanip>
#include <algorithm>
#include <iterator>
#include <functional>
#include <sstream>
#include <cctype>

struct Point {
    int x, y;

    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }

    bool operator!=(const Point& other) const {
        return !(*this == other);
    }
};

struct Polygon {
    std::vector<Point> points;

    bool operator==(const Polygon& other) const {
        if (points.size() != other.points.size()) {
            return false;
        }
        return std::equal(points.begin(), points.end(), other.points.begin());
    }

    bool operator!=(const Polygon& other) const {
        return !(*this == other);
    }
};

bool isValidPolygon(const Polygon& poly) {
    return poly.points.size() >= 3;
}

std::istream& operator>>(std::istream& in, Point& p) {
    char c1, c2, c3;
    if (in >> c1 && c1 == '(' && in >> p.x &&
        in >> c2 && c2 == ';' && in >> p.y &&
        in >> c3 && c3 == ')') {
    }
    else {
        in.setstate(std::ios::failbit);
    }
    return in;
}

struct AreaCalculator {
    const Polygon& poly;
    size_t n;
    size_t i = 0;

    double operator()(double sum, const Point& p1) {
        const Point& p2 = poly.points[(i + 1) % n];
        double step = (p1.x * p2.y) - (p1.y * p2.x);
        i++;
        return sum + step;
    }
};

double getArea(const Polygon& poly) {
    if (!isValidPolygon(poly)) {
        return 0.0;
    }

    double area = std::accumulate(poly.points.begin(), poly.points.end(), 0.0,
        AreaCalculator{ poly, poly.points.size() });

    return std::abs(area) / 2.0;
}

struct IsNotDigit {
    bool operator()(char c) const {
        return c < '0' || c > '9';
    }
};

bool isNumber(const std::string& s) {
    if (s.empty()) {
        return false;
    }

    return std::find_if(s.begin(), s.end(), IsNotDigit()) == s.end();
}

bool readPolygon(std::istringstream& iss, Polygon& poly) {
    if (!iss.good()) {
        return false;
    }

    size_t numPoints;
    if (!(iss >> numPoints)) {
        return false;
    }

    if (numPoints < 3) {
        return false;
    }

    std::vector<Point> points(numPoints);

    if (!iss.good()) {
        return false;
    }

    std::copy_n(std::istream_iterator<Point>(iss), numPoints, points.begin());

    if (iss.fail()) {
        return false;
    }

    char leftover;
    if (iss >> leftover) {
        return false;
    }

    poly.points = points;
    return true;
}

bool isRectangle(const Polygon& poly) {
    if (poly.points.size() != 4) {
        return false;
    }

    int x1 = poly.points[1].x - poly.points[0].x;
    int y1 = poly.points[1].y - poly.points[0].y;

    int x2 = poly.points[2].x - poly.points[1].x;
    int y2 = poly.points[2].y - poly.points[1].y;

    int x3 = poly.points[3].x - poly.points[2].x;
    int y3 = poly.points[3].y - poly.points[2].y;

    int x4 = poly.points[0].x - poly.points[3].x;
    int y4 = poly.points[0].y - poly.points[3].y;

    bool opposite1 = (x1 == -x3) && (y1 == -y3);
    bool opposite2 = (x2 == -x4) && (y2 == -y4);

    if (!opposite1 || !opposite2) {
        return false;
    }

    int dot1 = x1 * x2 + y1 * y2;
    int dot2 = x2 * x3 + y2 * y3;

    return (dot1 == 0) && (dot2 == 0);
}

struct IsEvenVertex {
    bool operator()(const Polygon& p) const {
        return p.points.size() % 2 == 0;
    }
};

struct IsOddVertex {
    bool operator()(const Polygon& p) const {
        return p.points.size() % 2 == 1;
    }
};

struct CompareByArea {
    bool operator()(const Polygon& a, const Polygon& b) const {
        return getArea(a) < getArea(b);
    }
};

struct SumArea {
    double operator()(double sum, const Polygon& p) const {
        return sum + getArea(p);
    }
};

struct SumAreaIfEven {
    double operator()(double sum, const Polygon& p) const {
        if (p.points.size() % 2 == 0) {
            return sum + getArea(p);
        }
        return sum;
    }
};

struct SumAreaIfOdd {
    double operator()(double sum, const Polygon& p) const {
        if (p.points.size() % 2 == 1) {
            return sum + getArea(p);
        }
        return sum;
    }
};

struct SumAreaIfVertexCount {
    size_t target;
    SumAreaIfVertexCount(size_t t) : target(t) {}

    double operator()(double sum, const Polygon& p) const {
        if (p.points.size() == target) {
            return sum + getArea(p);
        }
        return sum;
    }
};

struct CountIfVertexCount {
    size_t target;
    CountIfVertexCount(size_t t) : target(t) {}

    bool operator()(const Polygon& p) const {
        return p.points.size() == target;
    }
};

struct CompareByVertexCount {
    bool operator()(const Polygon& a, const Polygon& b) const {
        return a.points.size() < b.points.size();
    }
};

struct PointReader {
    std::ifstream& file;
    bool& ok;

    PointReader(std::ifstream& f, bool& o) : file(f), ok(o) {}

    Point operator()(const Point&) const {
        Point p;
        if (ok && !(file >> p)) {
            ok = false;
        }
        return p;
    }
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Error: filename not specified" << '\n';
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Error: cannot open file" << '\n';
        return 1;
    }

    std::vector<Polygon> shapes;

    size_t numPoints;

    while (file >> numPoints) {
        Polygon poly;
        bool ok = true;

        poly.points.resize(numPoints);

        std::transform(poly.points.begin(), poly.points.end(), poly.points.begin(),
            PointReader(file, ok));

        if (!ok) {
            poly.points.clear();
        }

        if (ok && isValidPolygon(poly)) {
            shapes.push_back(poly);
        }
        else {
            file.clear();
            file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    std::string cmd;

    std::cout << std::fixed << std::setprecision(1);

    while (std::cin >> cmd) {

        if (cmd == "AREA") {
            std::string arg;
            std::cin >> arg;

            double total = 0.0;

            if (arg == "EVEN") {
                total = std::accumulate(shapes.begin(), shapes.end(), 0.0, SumAreaIfEven());
                std::cout << total << '\n';
            }

            else if (arg == "ODD") {
                total = std::accumulate(shapes.begin(), shapes.end(), 0.0, SumAreaIfOdd());
                std::cout << total << '\n';
            }

            else if (arg == "MEAN") {
                if (shapes.empty()) {
                    std::cout << "<INVALID COMMAND>" << '\n';
                }
                else {
                    double sumAll = std::accumulate(shapes.begin(), shapes.end(), 0.0,
                        SumArea());
                    std::cout << sumAll / shapes.size() << '\n';
                }
            }

            else if (isNumber(arg) == true) {
                int num = std::stoi(arg);

                if (num < 3) {
                    std::cout << "<INVALID COMMAND>" << '\n';
                }
                else {
                    double res = std::accumulate(shapes.begin(), shapes.end(), 0.0,
                        SumAreaIfVertexCount(static_cast<size_t>(num)));
                    std::cout << res << '\n';
                }
            }

            else {
                std::cout << "<INVALID COMMAND>" << '\n';
            }
        }

        else if (cmd == "COUNT") {
            std::string arg;
            std::cin >> arg;

            if (shapes.empty()) {
                if (arg == "EVEN" || arg == "ODD") {
                    std::cout << 0 << '\n';
                }
                else if (isNumber(arg)) {
                    int num = std::stoi(arg);
                    if (num < 3) {
                        std::cout << "<INVALID COMMAND>" << '\n';
                    }
                    else {
                        std::cout << 0 << '\n';
                    }
                }
                else {
                    std::cout << "<INVALID COMMAND>" << '\n';
                }
            }

            else if (arg == "EVEN") {
                size_t count = std::count_if(shapes.begin(), shapes.end(), IsEvenVertex());
                std::cout << count << '\n';
            }

            else if (arg == "ODD") {
                size_t count = std::count_if(shapes.begin(), shapes.end(), IsOddVertex());
                std::cout << count << '\n';
            }

            else if (isNumber(arg)) {
                int num = std::stoi(arg);
                if (num < 3) {
                    std::cout << "<INVALID COMMAND>" << '\n';
                }
                else {
                    size_t count = std::count_if(shapes.begin(), shapes.end(),
                        CountIfVertexCount(static_cast<size_t>(num)));
                    std::cout << count << '\n';
                }
            }

            else {
                std::cout << "<INVALID COMMAND>" << '\n';
            }
        }

        else if (cmd == "MAX" || cmd == "MIN") {
            std::string arg;
            std::cin >> arg;

            if (shapes.empty()) {
                std::cout << "<INVALID COMMAND>" << '\n';
            }

            else if (arg == "AREA") {

                if (cmd == "MAX") {
                    auto it = std::max_element(shapes.begin(), shapes.end(), CompareByArea());
                    std::cout << getArea(*it) << '\n';
                }

                else {
                    auto it = std::min_element(shapes.begin(), shapes.end(), CompareByArea());
                    std::cout << getArea(*it) << '\n';
                }
            }

            else if (arg == "VERTEXES") {

                if (cmd == "MAX") {
                    auto it = std::max_element(shapes.begin(), shapes.end(),
                        CompareByVertexCount());
                    std::cout << it->points.size() << '\n';
                }

                else {
                    auto it = std::min_element(shapes.begin(), shapes.end(),
                        CompareByVertexCount());
                    std::cout << it->points.size() << '\n';
                }
            }

            else {
                std::cout << "<INVALID COMMAND>" << '\n';
            }
        }

        else if (cmd == "MAXSEQ") {
            std::string line;
            std::getline(std::cin, line);

            std::istringstream iss(line);

            Polygon target;
            if (!readPolygon(iss, target)) {
                std::cout << "<INVALID COMMAND>" << '\n';
                continue;
            }

            char leftover;
            if (iss >> leftover) {
                std::cout << "<INVALID COMMAND>" << '\n';
                continue;
            }

            if (shapes.empty()) {
                std::cout << 0 << '\n';
                continue;
            }

            struct State {
                int current = 0;
                int max = 0;
            };

            State result = std::accumulate(shapes.begin(), shapes.end(), State{},
                [&target](State state, const Polygon& p) {
                    if (p == target) {
                        state.current++;
                        if (state.current > state.max) {
                            state.max = state.current;
                        }
                    }
                    else {
                        state.current = 0;
                    }
                    return state;
                });

            std::cout << result.max << '\n';
        }
        else if (cmd == "RECTS") {
            if (shapes.empty()) {
                std::cout << 0 << '\n';
            }
            else {
                size_t count = std::count_if(shapes.begin(), shapes.end(), isRectangle);
                std::cout << count << '\n';
            }
        }
        else if (cmd == "ECHO") {
            std::string line;
            std::getline(std::cin, line);
            std::istringstream iss(line);

            Polygon target;
            if (!readPolygon(iss, target)) {
                std::cout << "<INVALID COMMAND>" << '\n';
                continue;
            }

            int count = std::count_if(shapes.begin(), shapes.end(),
                std::bind(&Polygon::operator==, std::placeholders::_1, target));

            std::vector<Polygon> newShapes = std::accumulate(shapes.begin(), shapes.end(),
                std::vector<Polygon>(),
                [&target](std::vector<Polygon>& acc, const Polygon& p) {
                    acc.push_back(p);
                    if (p == target) {
                        acc.push_back(p);
                    }
                    return acc;
                });

            shapes = std::move(newShapes);
            std::cout << count << '\n';
        }
        else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "<INVALID COMMAND>" << '\n';
        }
    }

    return 0;
}
