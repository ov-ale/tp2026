#include <algorithm>
#include <cstddef>
#include <functional>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <istream>
#include <numeric>
#include <sstream>
#include <iterator>
#include <cmath>
#include <string>
#include <vector>

// variant 17: INFRAME MAXSEQ

using namespace std::placeholders;

struct Point {
    int x, y;
};

struct Polygon {
    std::vector<Point> points;
};

std::istream& operator>>(std::istream& in, Point& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) {
        return in;
    }

    char c;
    int x, y;

    in >> c;
    if (c != '(') {
        in.setstate(std::ios::failbit);
        return in;
    }

    in >> x >> c;
    if (c != ';') {
        in.setstate(std::ios::failbit);
        return in;
    }

    in >> y >> c;
    if (c != ')') {
        in.setstate(std::ios::failbit);
        return in;
    }

    dest = {x, y};
    return in;
}

std::istream& operator>>(std::istream& in, Polygon& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) {
        return in;
    }

    Polygon tmp;

    std::size_t number_of_points;
    in >> number_of_points;

    if (!in || number_of_points < 3) {
        in.setstate(std::ios::failbit);
        return in;
    }

    tmp.points.resize(number_of_points);

    std::copy_n(
        std::istream_iterator<Point>(in),
        number_of_points,
        tmp.points.begin()
    );

    if (!in) {
        return in;
    }

    dest = std::move(tmp);
    return in;
}

struct GaussSum {
    const std::vector<Point> &p;
    std::size_t n;
    std::size_t i;

    explicit GaussSum(const std::vector<Point>& p)
        : p(p)
        , n(p.size())
        , i(0)
    {}

    long long operator()(long long acc, const Point &curr) {
        const Point &next = p[(i + 1) % n];

        long long term =
            static_cast<long long>(curr.x) * next.y -
            static_cast<long long>(next.x) * curr.y;

        ++i;
        return acc + term;
    }
};

double area(const Polygon &poly) {
    const std::vector<Point> &p = poly.points;
    long long sum = 0;

    sum = std::accumulate(
        p.begin(),
        p.end(),
        0LL,
        GaussSum(p)
    );

    return std::abs(sum) / 2.0;
}

struct SumArea {
    double operator()(double acc, const Polygon &p) const {
        return acc + area(p);
    }
};

struct IsEven {
    bool operator()(const Polygon& p) const {
        return p.points.size() % 2 == 0;
    }
};

struct SumIfEven {
    double operator()(double acc, const Polygon &p) const {
        return IsEven()(p) ? acc + area(p) : acc;
    }
};

struct SumIfOdd {
    double operator()(double acc, const Polygon &p) const {
        return !(IsEven()(p)) ? acc + area(p) : acc;
    }
};

struct AreaIfVertexCount {
    std::size_t n;

    explicit AreaIfVertexCount(std::size_t n)
        : n(n)
    {}

    double operator()(double acc, const Polygon& p) const {
        return (p.points.size() == n) ? acc + area(p) : acc;
    }
};

struct AreaCompare {
    bool operator()(const Polygon &a, const Polygon &b) const {
        return area(a) < area(b);
    }
};

struct NumOfVertexesCompare {
    bool operator()(const Polygon &a, const Polygon &b) const {
        return a.points.size() < b.points.size();
    }
};

struct IsNumOfVertexes {
    std::size_t n;

    IsNumOfVertexes(std::size_t n)
        : n(n)
    {}

    bool operator()(const Polygon &p) {
        return p.points.size() == n;
    }
};

struct Frame {
    int x_min, x_max, y_min, y_max;
    bool initialized = false;
};

Frame get_local_frame(const Polygon& p) {
    const auto& points = p.points;

    auto compare_x = std::bind(
        std::less<int>(),
        std::bind(&Point::x, _1),
        std::bind(&Point::x, _2)
    );

    auto compare_y = std::bind(
        std::less<int>(),
        std::bind(&Point::y, _1),
        std::bind(&Point::y, _2)
    );

    auto x = std::minmax_element(
        points.begin(),
        points.end(),
        compare_x
    );

    auto y = std::minmax_element(
        points.begin(),
        points.end(),
        compare_y
    );

    return { x.first->x, x.second->x, y.first->y, y.second->y, true };
}

struct MergeFrames {
    Frame operator()(Frame a, Frame b) const {
        if (!a.initialized) return b;
        if (!b.initialized) return a;

        return {
            std::min(a.x_min, b.x_min),
            std::max(a.x_max, b.x_max),
            std::min(a.y_min, b.y_min),
            std::max(a.y_max, b.y_max),
            true
        };
    }
};

struct FindFrame {
    Frame operator()(Frame acc, const Polygon &p) const {
        return MergeFrames()(acc, get_local_frame(p));
    }
};

struct IsInsideFrame {
    Frame f;

    bool operator()(const Point& p) const {
        return p.x >= f.x_min && p.x <= f.x_max &&
               p.y >= f.y_min && p.y <= f.y_max;
    }
};

bool operator==(const Point &a, const Point &b) {
    return (a.x == b.x) && (a.y == b.y);
}

bool operator==(const Polygon &a, const Polygon &b) {
    if (a.points.size() == b.points.size()) {
        return std::equal(
                    a.points.begin(), a.points.end(),
                    b.points.begin()
                );
    }
    return false;
}

struct MaxSeqState {
    std::size_t cur;
    std::size_t best;
};

struct MaxSeqAccumulator {
    const Polygon& target;

    explicit MaxSeqAccumulator(const Polygon& p)
        : target(p)
    {}

    MaxSeqState operator()(MaxSeqState state, const Polygon& p) const {
        if (p == target) {
            ++state.cur;
            state.best = std::max(state.best, state.cur);
        } else {
            state.cur = 0;
        }
        return state;
    }
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "ERROR: No file name provided or incorrect file name\n";
        std::cout << "Usage: ./main <filename>\n";
        return 1;
    }

    std::string filename = argv[1];

    std::ifstream file(filename);
    if (!file) {
        std::cout << "ERROR: file cannot be open or where is no file with such name\n";
        std::cout << "Usage: ./main <filename>\n";
        return 1;
    }

    std::vector<Polygon> polygons;
    std::string raw;

    while (std::getline(file, raw)) {
        if (raw.empty()) continue;

        std::istringstream iss(raw);

        Polygon poly;
        if (iss >> poly) {
            iss >> std::ws;
            if (iss.eof()) {
                polygons.push_back(std::move(poly));
            }
        }
    }

    std::cout << std::fixed << std::setprecision(1);

    std::string command;
    while (std::getline(std::cin, command)) {
        if (command.empty()) continue;

        std::istringstream iss(command);

        std::string main_cmd;
        iss >> main_cmd;

        if (main_cmd == "AREA") {
            std::string sub_cmd;
            iss >> sub_cmd;

            if (iss >> std::ws && !iss.eof()) {
                std::cout << "<INVALID COMMAND>\n";
                continue;
            }

            if (sub_cmd == "EVEN") {
                double result = std::accumulate(
                    polygons.begin(),
                    polygons.end(),
                    0.0,
                    SumIfEven()
                );

                std::cout << result << "\n";
            } else if (sub_cmd == "ODD") {
                double result = std::accumulate(
                    polygons.begin(),
                    polygons.end(),
                    0.0,
                    SumIfOdd()
                );

                std::cout << result << "\n";
            } else if (sub_cmd == "MEAN") {
                if (polygons.empty()) {
                    std::cout << "<INVALID COMMAND>\n";
                    continue;
                }

                double result = std::accumulate(
                    polygons.begin(),
                    polygons.end(),
                    0.0,
                    SumArea()
                );

                std::cout << result / polygons.size() << "\n";
            } else {
                try {
                    std::size_t pos = 0;
                    std::size_t num_of_vertexes = std::stoul(sub_cmd, &pos);

                    if (sub_cmd[0] == '-') {
                        throw std::invalid_argument("ERROR: number of vertexes can't be negative");
                    }

                    if (pos != sub_cmd.size()) {
                        throw std::invalid_argument("ERROR: not pure number");
                    }

                    if (num_of_vertexes < 3) {
                        throw std::invalid_argument("ERROR: incorrect number of vertexes");
                    }

                    double area = std::accumulate(
                        polygons.begin(),
                        polygons.end(),
                        0.0,
                        AreaIfVertexCount(num_of_vertexes)
                    );

                    std::cout << area << "\n";
                } catch (...) {
                    std::cout << "<INVALID COMMAND>\n";
                    continue;
                }
            }
        } else if (main_cmd == "MAX") {
            std::string sub_cmd;
            iss >> sub_cmd;

            if (iss >> std::ws && !iss.eof()) {
                std::cout << "<INVALID COMMAND>\n";
                continue;
            }

            if (polygons.empty()) {
                std::cout << "<INVALID COMMAND>\n";
                continue;
            }

            if (sub_cmd == "AREA") {
                auto max_element_it = std::max_element(
                    polygons.begin(),
                    polygons.end(),
                    AreaCompare()
                );

                std::cout << area(*max_element_it) << "\n";
            } else if (sub_cmd == "VERTEXES") {
                auto max_element_it = std::max_element(
                    polygons.begin(),
                    polygons.end(),
                    NumOfVertexesCompare()
                );

                std::cout << max_element_it->points.size() << "\n";
            } else {
                std::cout << "<INVALID COMMAND>\n";
                continue;
            }
        } else if (main_cmd == "MIN") {
            std::string sub_cmd;
            iss >> sub_cmd;

            if (iss >> std::ws && !iss.eof()) {
                std::cout << "<INVALID COMMAND>\n";
                continue;
            }

            if (polygons.empty()) {
                std::cout << "<INVALID COMMAND>\n";
                continue;
            }

            if (sub_cmd == "AREA") {
                auto min_element_it = std::min_element(
                    polygons.begin(),
                    polygons.end(),
                    AreaCompare()
                );

                std::cout << area(*min_element_it) << "\n";
            } else if (sub_cmd == "VERTEXES") {
                auto min_element_it = std::min_element(
                    polygons.begin(),
                    polygons.end(),
                    NumOfVertexesCompare()
                );

                std::cout << min_element_it->points.size() << "\n";
            } else {
                std::cout << "<INVALID COMMAND>\n";
                continue;
            }
        } else if (main_cmd == "COUNT") {
            std::string sub_cmd;
            iss >> sub_cmd;

            if (iss >> std::ws && !iss.eof()) {
                std::cout << "<INVALID COMMAND>\n";
                continue;
            }

            if (sub_cmd == "EVEN") {
                std::size_t cnt = std::count_if(
                    polygons.begin(),
                    polygons.end(),
                    IsEven()
                );

                std::cout << cnt << "\n";
            } else if (sub_cmd == "ODD") {
                std::size_t cnt = std::count_if(
                    polygons.begin(),
                    polygons.end(),
                    std::bind(std::logical_not<bool>(), std::bind(IsEven(), _1))
                );

                std::cout << cnt << "\n";
            } else {
                try {
                    std::size_t pos = 0;
                    std::size_t num_of_vertexes = std::stoul(sub_cmd, &pos);

                    if (pos != sub_cmd.size()) {
                        throw std::invalid_argument("ERROR: not pure number");
                    }

                    if (num_of_vertexes < 3) {
                        throw std::invalid_argument("ERROR: incorrect number of vertexes");
                    }

                    std::size_t cnt = std::count_if(
                        polygons.begin(),
                        polygons.end(),
                        IsNumOfVertexes(num_of_vertexes)
                    );

                    std::cout << cnt << "\n";
                } catch (...) {
                    std::cout << "<INVALID COMMAND>\n";
                    continue;
                }
            }
        } else if (main_cmd == "INFRAME") {
            if (polygons.empty()) {
                std::cout << "<INVALID COMMAND>\n";
                continue;
            }

            Polygon poly;
            iss >> poly;

            if (!iss || (iss >> std::ws && !iss.eof())) {
                std::cout << "<INVALID COMMAND>\n";
                continue;
            }

            Frame res = std::accumulate(
                polygons.begin(),
                polygons.end(),
                Frame(),
                FindFrame()
            );

            bool ok = std::all_of(
                poly.points.begin(),
                poly.points.end(),
                IsInsideFrame{res}
            );

            std::cout << (ok ? "<TRUE>\n" : "<FALSE>\n");
        } else if (main_cmd == "MAXSEQ") {
            Polygon poly;
            iss >> poly;

            if (!iss || (iss >> std::ws && !iss.eof())) {
                std::cout << "<INVALID COMMAND>\n";
                continue;
            }

            if (polygons.empty()) {
                std::cout << "0\n";
                continue;
            }

            MaxSeqState res = std::accumulate(
                polygons.begin(),
                polygons.end(),
                MaxSeqState{0, 0},
                MaxSeqAccumulator(poly)
            );

            std::cout << res.best << "\n";
        } else {
            std::cout << "<INVALID COMMAND>\n";
        }
    }

    return 0;
}
