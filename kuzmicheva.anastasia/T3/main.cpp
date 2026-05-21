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
        for (size_t i = 0; i < points.size(); ++i) {
            if (points[i] != other.points[i]) {
                return false;
            }
        }
        return true;
    }

    bool operator!=(const Polygon& other) const {
        return !(*this == other);
    }
};

bool isValidPolygon(const Polygon& poly) {
    return poly.points.size() >= 3;
}

bool readPointDirect(std::ifstream& file, Point& p) {
    char c1, c2, c3;
    if (file >> c1 && c1 == '(' && file >> p.x &&
        file >> c2 && c2 == ';' && file >> p.y &&
        file >> c3 && c3 == ')')
    {
        return true;
    }
    return false;
}

double getArea(const Polygon& poly) {
    if (!isValidPolygon(poly)) {
        return 0.0;
    }

    double area = 0.0;
    int n = poly.points.size();

    for (int i = 0; i < n; ++i) {
        area += (poly.points[i].x * poly.points[(i + 1) % n].y);
        area -= (poly.points[(i + 1) % n].x * poly.points[i].y);
    }

    return std::abs(area) / 2.0;
}

bool isNumber(const std::string& s) {
    if (s.empty()) {
        return false;
    }

    for (size_t i = 0; i < s.length(); ++i) {
        if (s[i] < '0' || s[i] > '9') {
            return false;
        }
    }
    return true;
}

bool readPolygon(std::istringstream& iss, Polygon& poly) {
    size_t numPoints;
    if (!(iss >> numPoints)) {
        return false;
    }

    if (numPoints < 3) {
        return false;
    }

    std::vector<Point> points;
    for (size_t i = 0; i < numPoints; ++i) {
        char c1, c2, c3;
        int x, y;
        if (iss >> c1 && c1 == '(' && iss >> x &&
            iss >> c2 && c2 == ';' && iss >> y &&
            iss >> c3 && c3 == ')') {
            points.push_back({ x, y });
        }
        else {
            return false;
        }
    }

    char leftover;
    if (iss >> leftover) {
        return false;
    }

    if (points.size() == numPoints) {
        poly.points = points;
        return true;
    }
    return false;
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

        for (size_t i = 0; i < numPoints; ++i) {
            Point p;
            if (!readPointDirect(file, p)) {
                ok = false;
                break;
            }
            poly.points.push_back(p);
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

            if (isNumber(arg)) {
                int num = std::stoi(arg);
                if (num < 3) {
                    std::cout << "<INVALID COMMAND>" << '\n';
                    continue;
                }
            }

            double total = 0.0;

            if (arg == "EVEN") {
                total = std::accumulate(shapes.begin(), shapes.end(), 0.0,
                    [](double sum, const Polygon& p) {
                        if (p.points.size() % 2 == 0) {
                            return sum + getArea(p);
                        }
                        return sum;
                    });
                std::cout << total << '\n';
            }

            else if (arg == "ODD") {
                total = std::accumulate(shapes.begin(), shapes.end(), 0.0,
                    [](double sum, const Polygon& p) {
                        if (p.points.size() % 2 == 1) {
                            return sum + getArea(p);
                        }
                        return sum;
                    });
                std::cout << total << '\n';
            }

            else if (arg == "MEAN") {
                if (shapes.empty()) {
                    std::cout << "<INVALID COMMAND>" << '\n';
                }
                else {
                    double sumAll = std::accumulate(shapes.begin(), shapes.end(), 0.0,
                        [](double sum, const Polygon& p) {
                            return sum + getArea(p);
                        });
                    std::cout << sumAll / shapes.size() << '\n';
                }
            }

            else if (isNumber(arg) == true) {
                int num = std::stoi(arg);

                double res = std::accumulate(shapes.begin(), shapes.end(), 0.0,
                    [num](double sum, const Polygon& p) {
                        if (p.points.size() == static_cast<size_t>(num)) {
                            return sum + getArea(p);
                        }
                        return sum;
                    });

                std::cout << res << '\n';
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
                size_t count = std::count_if(shapes.begin(), shapes.end(),
                    [](const Polygon& p) {
                        return p.points.size() % 2 == 0;
                    });
                std::cout << count << '\n';
            }

            else if (arg == "ODD") {
                size_t count = std::count_if(shapes.begin(), shapes.end(),
                    [](const Polygon& p) {
                        return p.points.size() % 2 == 1;
                    });
                std::cout << count << '\n';
            }

            else if (isNumber(arg)) {
                int num = std::stoi(arg);
                if (num < 3) {
                    std::cout << "<INVALID COMMAND>" << '\n';
                }
                else {
                    size_t count = std::count_if(shapes.begin(), shapes.end(),
                        [num](const Polygon& p) {
                            return p.points.size() == static_cast<size_t>(num);
                        });
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
                    auto it = std::max_element(shapes.begin(), shapes.end(),
                        [](const Polygon& a, const Polygon& b) {
                            return getArea(a) < getArea(b);
                        });
                    std::cout << getArea(*it) << '\n';
                }

                else {
                    auto it = std::min_element(shapes.begin(), shapes.end(),
                        [](const Polygon& a, const Polygon& b) {
                            return getArea(a) < getArea(b);
                        });
                    std::cout << getArea(*it) << '\n';
                }
            }

            else if (arg == "VERTEXES") {

                if (cmd == "MAX") {
                    auto it = std::max_element(shapes.begin(), shapes.end(),
                        [](const Polygon& a, const Polygon& b) {
                            return a.points.size() < b.points.size();
                        });
                    std::cout << it->points.size() << '\n';
                }

                else {
                    auto it = std::min_element(shapes.begin(), shapes.end(),
                        [](const Polygon& a, const Polygon& b) {
                            return a.points.size() < b.points.size();
                        });
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

            int count = 0;
            for (const auto& poly : shapes) {
                if (poly == target) {
                    count++;
                }
            }

            std::vector<Polygon> newShapes;
            for (const auto& poly : shapes) {
                newShapes.push_back(poly);
                if (poly == target) {
                    newShapes.push_back(poly);
                }
            }
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
