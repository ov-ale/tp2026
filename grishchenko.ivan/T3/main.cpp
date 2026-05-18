#include <iostream>
#include <string>
#include <functional>
#include <algorithm>
#include <fstream>
#include <numeric>
#include <iomanip>
#include <cmath>
#include <vector>
#include <utility>
#include <limits>
#include <iterator>
#include <sstream>

using namespace std::placeholders;

struct Point {
    int x, y;
};

struct Polygon {
    std::vector<Point> points;
};

bool operator==(const Point& p1, const Point& p2) {
    return p1.x == p2.x && p1.y == p2.y;
}

bool operator==(const Polygon& p1, const Polygon& p2) {
    return p1.points == p2.points;
}

std::istream& operator>>(std::istream& is, Point& p) {
    std::istream::sentry sentry(is);
    if (!is) return is;
    char c1, c2, c3;
    if (is >> c1 >> p.x >> c2 >> p.y >> c3) {
        if (c1 == '(' && c2 == ';' && c3 == ')') {
            return is;
        }
    }
    is.setstate(std::ios_base::failbit);
    return is;
}

std::istream& operator>>(std::istream& is, Polygon& pg) {
    std::istream::sentry sentry(is);
    if (!is) return is;
    size_t count;
    if (!(is >> count) || count < 3) {
        is.setstate(std::ios_base::failbit);
        return is;
    }
    std::vector<Point> pts;
    std::copy_n(std::istream_iterator<Point>(is), count, std::back_inserter(pts));
    if (is) {
        pg.points = pts;
    }
    return is;
}

double calcAreaTerm(Point a, Point b) {
    return static_cast<double>(a.x * b.y - a.y * b.x);
}

double area(const Polygon& pg) {
    if (pg.points.size() < 3) return 0.0;
    std::vector<double> terms;
    std::transform(pg.points.begin(), pg.points.end() - 1, pg.points.begin() + 1, std::back_inserter(terms), calcAreaTerm);
    terms.push_back(calcAreaTerm(pg.points.back(), pg.points.front()));
    double sum = std::accumulate(terms.begin(), terms.end(), 0.0);
    return std::abs(sum) / 2.0;
}

bool isEvenFunc(const Polygon& pg) { return pg.points.size() % 2 == 0; }
bool isOddFunc(const Polygon& pg) { return pg.points.size() % 2 != 0; }
bool hasNumVertices(const Polygon& pg, size_t n) { return pg.points.size() == n; }

double addAreaIfEven(double sum, const Polygon& p) { return isEvenFunc(p) ? sum + area(p) : sum; }
double addAreaIfOdd(double sum, const Polygon& p) { return isOddFunc(p) ? sum + area(p) : sum; }
double addAreaIfNum(double sum, const Polygon& p, size_t n) { return hasNumVertices(p, n) ? sum + area(p) : sum; }
double addArea(double sum, const Polygon& p) { return sum + area(p); }

bool compareArea(const Polygon& p1, const Polygon& p2) { return area(p1) < area(p2); }
bool compareVertexes(const Polygon& p1, const Polygon& p2) { return p1.points.size() < p2.points.size(); }

struct MaxSeqState {
    size_t current_run = 0;
    size_t max_run = 0;
};

MaxSeqState accumulateMaxSeq(MaxSeqState state, const Polygon& p, const Polygon& target) {
    if (p == target) {
        state.current_run++;
        state.max_run = std::max(state.max_run, state.current_run);
    } else {
        state.current_run = 0;
    }
    return state;
}

std::vector<Polygon> accumulateEcho(std::vector<Polygon> acc, const Polygon& p, const Polygon& target) {
    acc.push_back(p);
    if (p == target) {
        acc.push_back(p);
    }
    return acc;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>\n";
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file) {
        std::cerr << "Error: Cannot open file " << argv[1] << "\n";
        return 1;
    }

    std::vector<Polygon> polys;
    std::string file_line;
    while (std::getline(file, file_line)) {
        if (file_line.empty()) continue;
        std::stringstream ss(file_line);
        Polygon p;
        if (ss >> p) {
            std::string dummy;
            if (!(ss >> dummy)) {
                polys.push_back(p);
            }
        }
    }
    file.close();

    std::string input_line;
    while (std::getline(std::cin, input_line)) {
        if (input_line.empty()) continue;
        std::stringstream ss(input_line);
        std::string cmd;
        if (!(ss >> cmd)) continue;

        if (cmd == "AREA") {
            std::string subcmd;
            if (!(ss >> subcmd)) { std::cout << "<INVALID COMMAND>\n"; continue; }

            double res = 0.0;
            if (subcmd == "EVEN") {
                res = std::accumulate(polys.begin(), polys.end(), 0.0, addAreaIfEven);
            } else if (subcmd == "ODD") {
                res = std::accumulate(polys.begin(), polys.end(), 0.0, addAreaIfOdd);
            } else if (subcmd == "MEAN") {
                if (polys.empty()) { std::cout << "<INVALID COMMAND>\n"; continue; }
                res = std::accumulate(polys.begin(), polys.end(), 0.0, addArea) / polys.size();
            } else {
                try {
                    size_t n = std::stoull(subcmd);
                    if (n < 3) throw std::invalid_argument("");
                    res = std::accumulate(polys.begin(), polys.end(), 0.0, std::bind(addAreaIfNum, _1, _2, n));
                } catch (...) {
                    std::cout << "<INVALID COMMAND>\n";
                    continue;
                }
            }
            std::cout << std::fixed << std::setprecision(1) << res << "\n";

        } else if (cmd == "MIN") {
            std::string subcmd;
            if (!(ss >> subcmd) || polys.empty()) { std::cout << "<INVALID COMMAND>\n"; continue; }

            if (subcmd == "AREA") {
                auto it = std::min_element(polys.begin(), polys.end(), compareArea);
                std::cout << std::fixed << std::setprecision(1) << area(*it) << "\n";
            } else if (subcmd == "VERTEXES") {
                auto it = std::min_element(polys.begin(), polys.end(), compareVertexes);
                std::cout << it->points.size() << "\n";
            } else {
                std::cout << "<INVALID COMMAND>\n";
            }

        } else if (cmd == "MAX") {
            std::string subcmd;
            if (!(ss >> subcmd) || polys.empty()) { std::cout << "<INVALID COMMAND>\n"; continue; }

            if (subcmd == "AREA") {
                auto it = std::max_element(polys.begin(), polys.end(), compareArea);
                std::cout << std::fixed << std::setprecision(1) << area(*it) << "\n";
            } else if (subcmd == "VERTEXES") {
                auto it = std::max_element(polys.begin(), polys.end(), compareVertexes);
                std::cout << it->points.size() << "\n";
            } else {
                std::cout << "<INVALID COMMAND>\n";
            }

        } else if (cmd == "COUNT") {
            std::string subcmd;
            if (!(ss >> subcmd)) { std::cout << "<INVALID COMMAND>\n"; continue; }

            if (subcmd == "EVEN") {
                std::cout << std::count_if(polys.begin(), polys.end(), isEvenFunc) << "\n";
            } else if (subcmd == "ODD") {
                std::cout << std::count_if(polys.begin(), polys.end(), isOddFunc) << "\n";
            } else {
                try {
                    size_t n = std::stoull(subcmd);
                    if (n < 3) throw std::invalid_argument("");
                    std::cout << std::count_if(polys.begin(), polys.end(), std::bind(hasNumVertices, _1, n)) << "\n";
                } catch (...) {
                    std::cout << "<INVALID COMMAND>\n";
                }
            }

        } else if (cmd == "MAXSEQ") {
            Polygon target;
            std::string dummy;
            if (!(ss >> target) || (ss >> dummy)) {
                std::cout << "<INVALID COMMAND>\n";
                continue;
            }

            MaxSeqState init_state;
            MaxSeqState final_state = std::accumulate(
                polys.begin(), polys.end(), init_state,
                std::bind(accumulateMaxSeq, _1, _2, std::cref(target))
            );
            std::cout << final_state.max_run << "\n";

        } else if (cmd == "ECHO") {
            Polygon target;
            std::string dummy;
            if (!(ss >> target) || (ss >> dummy)) {
                std::cout << "<INVALID COMMAND>\n";
                continue;
            }

            size_t added_count = std::count(polys.begin(), polys.end(), target);

            std::vector<Polygon> updated_polys;
            updated_polys.reserve(polys.size() + added_count);

            updated_polys = std::accumulate(
                polys.begin(), polys.end(), updated_polys,
                std::bind(accumulateEcho, _1, _2, std::cref(target))
            );

            polys = std::move(updated_polys);
            std::cout << added_count << "\n";

        } else {
            std::cout << "<INVALID COMMAND>\n";
        }
    }

    return 0;
}
