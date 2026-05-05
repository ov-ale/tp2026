// VAR 8: RMECHO and INFRAME

#include <algorithm>
#include <iostream>
#include <iterator>
#include <sstream>
#include <vector>
#include <iomanip>
#include <string>
#include <cmath>
#include <cstddef>
#include <functional>
#include <fstream>
#include <istream>
#include <numeric>


using namespace std::placeholders;

struct Point {
    int x, y;
};

struct Polygon {
    std::vector<Point> points;
};

bool operator==(const Point& a, const Point& b) {
    return a.x == b.x && a.y == b.y;
}

bool operator==(const Polygon& a, const Polygon& b) {
    return a.points.size() == b.points.size() &&
        std::equal(a.points.begin(), a.points.end(), b.points.begin());
}

std::istream& operator>>(std::istream& in, Point& dest) {
    std::istream::sentry guard(in);
    if (!guard) return in;

    char ch;
    int x, y;

    in >> ch;
    if (ch != '(') {
        in.setstate(std::ios::failbit);
        return in;
    }

    in >> x >> ch;
    if (ch != ';') {
        in.setstate(std::ios::failbit);
        return in;
    }

    in >> y >> ch;
    if (ch != ')') {
        in.setstate(std::ios::failbit);
        return in;
    }

    dest = { x, y };
    return in;
}

std::istream& operator>>(std::istream& in, Polygon& dest) {
    std::istream::sentry guard(in);
    if (!guard) return in;

    Polygon temp;
    std::size_t count;
    in >> count;

    if (!in || count < 3) {
        in.setstate(std::ios::failbit);
        return in;
    }

    temp.points.resize(count);
    std::copy_n(std::istream_iterator<Point>(in), count, temp.points.begin());

    if (!in) return in;

    dest = std::move(temp);
    return in;
}

struct GaussCalc {
    const std::vector<Point>& pts;
    std::size_t size;
    std::size_t idx;

    explicit GaussCalc(const std::vector<Point>& pts) :
        pts(pts), size(pts.size()), idx(0) {
    }

    long long operator()(long long acc, const Point& curr) {
        const Point& next = pts[(idx + 1) % size];
        long long term = static_cast<long long>(curr.x) * next.y
            - static_cast<long long>(next.x) * curr.y;
        ++idx;
        return acc + term;
    }
};

double computeArea(const Polygon& poly) {
    if (poly.points.size() < 3) return 0.0;
    long long sum = std::accumulate(
        poly.points.begin(), poly.points.end(), 0LL,
        GaussCalc(poly.points)
    );
    return std::abs(sum) / 2.0;
}

struct IsEvenVertex {
    bool operator()(const Polygon& p) const {
        return p.points.size() % 2 == 0;
    }
};

struct HasVertexCount {
    std::size_t target;
    explicit HasVertexCount(std::size_t n) : target(n) {}
    bool operator()(const Polygon& p) const {
        return p.points.size() == target;
    }
};

struct SumArea {
    double operator()(double acc, const Polygon& p) const {
        return acc + computeArea(p);
    }
};

struct SumAreaIfEven {
    double operator()(double acc, const Polygon& p) const {
        return IsEvenVertex()(p) ? acc + computeArea(p) : acc;
    }
};

struct SumAreaIfOdd {
    double operator()(double acc, const Polygon& p) const {
        return !IsEvenVertex()(p) ? acc + computeArea(p) : acc;
    }
};

struct SumAreaIfVertexCount {
    std::size_t n;
    explicit SumAreaIfVertexCount(std::size_t n) : n(n) {}
    double operator()(double acc, const Polygon& p) const {
        return (p.points.size() == n) ? acc + computeArea(p) : acc;
    }
};

struct CompareByArea {
    bool operator()(const Polygon& a, const Polygon& b) const {
        return computeArea(a) < computeArea(b);
    }
};

struct CompareByVertices {
    bool operator()(const Polygon& a, const Polygon& b) const {
        return a.points.size() < b.points.size();
    }
};

struct BothEqualToTarget {
    Polygon target;
    explicit BothEqualToTarget(const Polygon& t) : target(t) {}
    bool operator()(const Polygon& a, const Polygon& b) const {
        return (a == target) && (b == target);
    }
};

struct BoundingBox {
    int minX, maxX, minY, maxY;
    bool valid;
};

struct CompareByX {
    bool operator()(const Point& a, const Point& b) const {
        return a.x < b.x;
    }
};

struct CompareByY {
    bool operator()(const Point& a, const Point& b) const {
        return a.y < b.y;
    }
};

BoundingBox computeBoundingBox(const Polygon& poly) {
    const auto& pts = poly.points;

    auto xRange = std::minmax_element(pts.begin(), pts.end(), CompareByX());
    auto yRange = std::minmax_element(pts.begin(), pts.end(), CompareByY());

    BoundingBox box;
    box.minX = xRange.first->x;
    box.maxX = xRange.second->x;
    box.minY = yRange.first->y;
    box.maxY = yRange.second->y;
    box.valid = true;
    return box;
}

struct MergeBoxes {
    BoundingBox operator()(const BoundingBox& a, const BoundingBox& b) const {
        if (!a.valid) return b;
        if (!b.valid) return a;

        BoundingBox result;
        result.minX = std::min(a.minX, b.minX);
        result.maxX = std::max(a.maxX, b.maxX);
        result.minY = std::min(a.minY, b.minY);
        result.maxY = std::max(a.maxY, b.maxY);
        result.valid = true;
        return result;
    }
};

struct BuildGlobalBox {
    BoundingBox operator()(const BoundingBox& acc, const Polygon& poly) const {
        return MergeBoxes()(acc, computeBoundingBox(poly));
    }
};

BoundingBox getGlobalBoundingBox(const std::vector<Polygon>& polygons) {
    return std::accumulate(
        polygons.begin(),
        polygons.end(),
        BoundingBox{0, 0, 0, 0, false},
        BuildGlobalBox()
    );
}

struct PointInBox {
    const BoundingBox& box;

    explicit PointInBox(const BoundingBox& b) : box(b) {}

    bool operator()(const Point& pt) const {
        return pt.x >= box.minX && pt.x <= box.maxX &&
            pt.y >= box.minY && pt.y <= box.maxY;
    }
};

bool isPolygonInside(const Polygon& poly, const BoundingBox& box) {
    return std::all_of(
        poly.points.begin(),
        poly.points.end(),
        PointInBox(box)
    );
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Error: filename required\n";
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file) {
        std::cerr << "Error: cannot open file\n";
        return 1;
    }

    std::vector<Polygon> storage;
    std::string line;

    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::istringstream iss(line);
        Polygon poly;
        if (iss >> poly) {
            iss >> std::ws;
            if (iss.eof()) {
                storage.push_back(std::move(poly));
            }
        }
    }
    file.close();

    std::cout << std::fixed << std::setprecision(1);

    std::string input;
    while (std::getline(std::cin, input)) {
        if (input.empty()) continue;

        std::istringstream iss(input);
        std::string cmd;
        iss >> cmd;

        if (cmd == "AREA") {
            std::string param;
            iss >> param;

            if (iss >> std::ws && !iss.eof()) {
                std::cout << "<INVALID COMMAND>\n";
                continue;
            }

            if (param == "EVEN") {
                double res = std::accumulate(storage.begin(), storage.end(), 0.0, SumAreaIfEven());
                std::cout << res << "\n";
            }
            else if (param == "ODD") {
                double res = std::accumulate(storage.begin(), storage.end(), 0.0, SumAreaIfOdd());
                std::cout << res << "\n";
            }
            else if (param == "MEAN") {
                if (storage.empty()) {
                    std::cout << "<INVALID COMMAND>\n";
                    continue;
                }
                double total = std::accumulate(storage.begin(), storage.end(), 0.0, SumArea());
                std::cout << total / storage.size() << "\n";
            }
            else {
                try {
                    std::size_t pos = 0;
                    std::size_t vcount = std::stoul(param, &pos);

                    if (pos != param.size() || vcount < 3) {
                        throw std::invalid_argument("");
                    }

                    double res = std::accumulate(storage.begin(), storage.end(), 0.0,
                        SumAreaIfVertexCount(vcount));
                    std::cout << res << "\n";
                }
                catch (...) {
                    std::cout << "<INVALID COMMAND>\n";
                }
            }
        }
        else if (cmd == "MAX") {
            std::string param;
            iss >> param;

            if ((iss >> std::ws && !iss.eof()) || storage.empty()) {
                std::cout << "<INVALID COMMAND>\n";
                continue;
            }

            if (param == "AREA") {
                auto it = std::max_element(storage.begin(), storage.end(), CompareByArea());
                std::cout << computeArea(*it) << "\n";
            }
            else if (param == "VERTEXES") {
                auto it = std::max_element(storage.begin(), storage.end(), CompareByVertices());
                std::cout << it->points.size() << "\n";
            }
            else {
                std::cout << "<INVALID COMMAND>\n";
            }
        }
        else if (cmd == "MIN") {
            std::string param;
            iss >> param;

            if ((iss >> std::ws && !iss.eof()) || storage.empty()) {
                std::cout << "<INVALID COMMAND>\n";
                continue;
            }

            if (param == "AREA") {
                auto it = std::min_element(storage.begin(), storage.end(), CompareByArea());
                std::cout << computeArea(*it) << "\n";
            }
            else if (param == "VERTEXES") {
                auto it = std::min_element(storage.begin(), storage.end(), CompareByVertices());
                std::cout << it->points.size() << "\n";
            }
            else {
                std::cout << "<INVALID COMMAND>\n";
            }
        }
        else if (cmd == "COUNT") {
            std::string param;
            iss >> param;

            if (iss >> std::ws && !iss.eof()) {
                std::cout << "<INVALID COMMAND>\n";
                continue;
            }

            if (param == "EVEN") {
                auto cnt = std::count_if(storage.begin(), storage.end(), IsEvenVertex());
                std::cout << cnt << "\n";
            }
            else if (param == "ODD") {
                auto cnt = std::count_if(storage.begin(), storage.end(),
                    std::bind(std::logical_not<bool>(), std::bind(IsEvenVertex(), _1)));
                std::cout << cnt << "\n";
            }
            else {
                try {
                    std::size_t pos = 0;
                    std::size_t vcount = std::stoul(param, &pos);

                    if (pos != param.size() || vcount < 3) {
                        throw std::invalid_argument("");
                    }

                    auto cnt = std::count_if(storage.begin(), storage.end(),
                        HasVertexCount(vcount));
                    std::cout << cnt << "\n";
                }
                catch (...) {
                    std::cout << "<INVALID COMMAND>\n";
                }
            }
        }
        else if (cmd == "RMECHO") {
            Polygon target;
            if (!(iss >> target)) {
                std::cout << "<INVALID COMMAND>\n";
                continue;
            }

            if (iss >> std::ws && !iss.eof()) {
                std::cout << "<INVALID COMMAND>\n";
                continue;
            }

            if (storage.empty()) {
                std::cout << "0\n";
                continue;
            }

            std::size_t oldSize = storage.size();
            auto newEnd = std::unique(storage.begin(), storage.end(),
                BothEqualToTarget(target));
            storage.erase(newEnd, storage.end());
            std::size_t removed = oldSize - storage.size();
            std::cout << removed << "\n";
        }
        else if (cmd == "INFRAME") {
            if (storage.empty()) {
                std::cout << "<INVALID COMMAND>\n";
                continue;
            }

            Polygon target;
            if (!(iss >> target)) {
                std::cout << "<INVALID COMMAND>\n";
                continue;
            }

            if (iss >> std::ws && !iss.eof()) {
                std::cout << "<INVALID COMMAND>\n";
                continue;
            }

            BoundingBox globalBox = getGlobalBoundingBox(storage);
            bool ok = isPolygonInside(target, globalBox);
            std::cout << (ok ? "<TRUE>\n" : "<FALSE>\n");
        }
        else {
            std::cout << "<INVALID COMMAND>\n";
        }
    }

    return 0;
}
