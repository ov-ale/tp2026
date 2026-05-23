// Variant 18: INFRAME INTERSECTIONS

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <functional>
#include <numeric>
#include <iomanip>
#include <cmath>

struct Point {
    int x, y;
};
struct Polygon {
    std::vector<Point> points;
};

std::istream& operator>>(std::istream& in, Point& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) return in;

    int x = 0;
    int y = 0;
    if (in.get() != '(') {
        in.setstate(std::ios::failbit);
        return in;
    }
    if (!(in >> x) || in.get() != ';') {
        in.setstate(std::ios::failbit);
        return in;
    }
    if (!(in >> y) || in.get() != ')') {
        in.setstate(std::ios::failbit);
        return in;
    }
    dest.x = x;
    dest.y = y;
    return in;
}

std::istream& operator>>(std::istream& in, Polygon& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) return in;

    Polygon temp;
    int Npoints = 0;
    if (!(in >> Npoints) || Npoints < 3) {
        in.setstate(std::ios::failbit);
        return in;
    }
    temp.points.resize(Npoints);
    std::copy_n(std::istream_iterator<Point>(in),
                Npoints,
                temp.points.begin()
    );

    if(!in) return in;

    dest = std::move(temp);
    return in;
}

struct GaussFormula {
    const std::vector<Point>& poly;
    std::size_t size;
    std::size_t i;

    explicit GaussFormula(const std::vector<Point>& poly) :
        poly(poly),
        size(poly.size()),
        i(0)
    {}

    long long operator()(long long acc, const Point& cur) {
        const Point& next = poly[(i + 1) % size];

        long long term =
            static_cast<long long>(cur.x) * next.y -
            static_cast<long long>(next.x) * cur.y;

        ++i;
        return acc + term;
    }
};

double getArea(const Polygon& poly) {
    long long sum = 0;
    sum = std::accumulate(
        poly.points.begin(),
        poly.points.end(),
        0LL,
        GaussFormula(poly.points)
    );
    return std::abs(sum) / 2.0;
}

struct SumArea {
    double operator()(double sum, const Polygon& poly) const {
        return sum + getArea(poly);
    }
};

struct SumAreaIfEven {
    double operator()(double sum, const Polygon& poly) const {
        if (poly.points.size() % 2 == 0) {
            return sum + getArea(poly);
        }
        return sum;
    }
};

struct SumAreaIfOdd {
    double operator()(double sum, const Polygon& poly) const {
        if (poly.points.size() % 2 != 0) {
            return sum + getArea(poly);
        }
        return sum;
    }
};

struct SumAreaIfNumOfVertexes {
    std::size_t n;

    explicit SumAreaIfNumOfVertexes(std::size_t n) :
        n(n)
    {}

    double operator()(double sum, const Polygon& poly) const {
        if (poly.points.size() == n) {
            return sum + getArea(poly);
        }
        return sum;
    }
};

struct CompareArea {
    bool operator()(const Polygon& first, const Polygon& second) const {
        return getArea(first) < getArea(second);
    }
};

struct CompareVertexes {
    bool operator()(const Polygon& first, const Polygon& second) const {
        return first.points.size() < second.points.size();
    }
};

struct IsEven {
    bool operator()(const Polygon& poly) const {
        return poly.points.size() % 2 == 0;
    }
};

struct IsOdd {
    bool operator()(const Polygon& poly) const {
        return poly.points.size() % 2 != 0;
    }
};

struct IsNumOfVertexes {
    std::size_t n;

    explicit IsNumOfVertexes(std::size_t n) :
        n(n)
    {}

    bool operator()(const Polygon& poly) const {
        return n == poly.points.size();
    }
};

struct Frame {
    int x_min, x_max, y_min, y_max;
};

struct CompareX {
    bool operator()(const Point& first, const Point& second) const {
        return first.x < second.x;
    }
};
struct CompareY {
    bool operator()(const Point& first, const Point& second) const {
        return first.y < second.y;
    }
};

Frame getFrameOfPolygon(const Polygon& poly) {
    auto x_min = std::min_element(poly.points.begin(), poly.points.end(), CompareX());
    auto x_max = std::max_element(poly.points.begin(), poly.points.end(), CompareX());
    auto y_min = std::min_element(poly.points.begin(), poly.points.end(), CompareY());
    auto y_max = std::max_element(poly.points.begin(), poly.points.end(), CompareY());
    return { x_min->x, x_max->x, y_min->y, y_max->y };
}

struct MergeFrames {
    Frame operator()(Frame acc, const Polygon& poly) const {
        return { std::min(acc.x_min, getFrameOfPolygon(poly).x_min),
                 std::max(acc.x_max, getFrameOfPolygon(poly).x_max),
                 std::min(acc.y_min, getFrameOfPolygon(poly).y_min),
                 std::max(acc.y_max, getFrameOfPolygon(poly).y_max),
        };
    }
};

Frame getFrame(const std::vector<Polygon>& polygons) {
    Frame initial = getFrameOfPolygon(polygons[0]);
    return std::accumulate(
        polygons.begin() + 1,
        polygons.end(),
        initial,
        MergeFrames()
    );
}

struct IsInsideFrame {
    Frame frame;

    explicit IsInsideFrame(const Frame& f) :
        frame(f)
    {}

    bool operator()(const Point& point) {
        return (point.x >= frame.x_min && point.x <= frame.x_max &&
                point.y >= frame.y_min && point.y <= frame.y_max);
    }
};

long long location(const Point& a, const Point& b, const Point& c) {
    return static_cast<long long>(b.x - a.x) * (c.y - a.y)
         - static_cast<long long>(b.y - a.y) * (c.x - a.x);
}

bool onSegment(const Point& a, const Point& b, const Point& c) {
    return c.x >= std::min(a.x, b.x) && c.x <= std::max(a.x, b.x) &&
           c.y >= std::min(a.y, b.y) && c.y <= std::max(a.y, b.y);
}

bool segmentsIntersect(const Point& a, const Point& b,
                       const Point& c, const Point& d) {
    long long location1 = location(c, d, a);
    long long location2 = location(c, d, b);
    long long location3 = location(a, b, c);
    long long location4 = location(a, b, d);

    if (((location1 > 0 && location2 < 0) || (location1 < 0 && location2 > 0)) &&
        ((location3 > 0 && location4 < 0) || (location3 < 0 && location4 > 0))) {
        return true;
    }
    if (location1 == 0 && onSegment(c, d, a)) return true;
    if (location2 == 0 && onSegment(c, d, b)) return true;
    if (location3 == 0 && onSegment(a, b, c)) return true;
    if (location4 == 0 && onSegment(a, b, d)) return true;

    return false;
}

bool IsPointInPolygon(const Point& point, const Polygon& poly) {
    int crossingCount = 0;

    for (std::size_t i = 0; i < poly.points.size(); ++i) {
        const Point& a = poly.points[i];
        const Point& b = poly.points[(i + 1) % poly.points.size()];

        if (a.y <= point.y && b.y > point.y && location(a, b, point) > 0) {
            ++crossingCount;
        }
        else if (a.y > point.y && b.y <= point.y && location(a, b, point) < 0) {
            --crossingCount;
        }
    }
    return crossingCount != 0;
}

bool polygonsIntersect(const Polygon& first, const Polygon& second) {
    for (std::size_t i = 0; i < first.points.size(); ++i) {
        for (std::size_t j = 0; j < second.points.size(); ++j) {
            if (segmentsIntersect(first.points[i], first.points[(i + 1) % first.points.size()],
                            second.points[j], second.points[(j + 1) % second.points.size()])) {
                return true;
            }
        }
    }
    if (IsPointInPolygon(first.points[0], second)) return true;
    if (IsPointInPolygon(second.points[0], first)) return true;

    return false;
}

struct IsIntersected {
    const Polygon& src;

    explicit IsIntersected(const Polygon& poly) :
        src(poly)
    {}

    bool operator()(const Polygon& poly) const {
        return polygonsIntersect(poly, src);
    }
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Add file with data\n";
        return 1;
    }
    std::ifstream file(argv[1]);
    if (!file) {
        std::cerr << "Cannot open the file\n";
        return 1;
    }

    std::vector<Polygon> polygons;
    std::string curString;

    while (std::getline(file, curString)) {
        if (curString.empty()) continue;

        std::istringstream iss(curString);
        Polygon curPolygon;
        if (iss >> curPolygon) {
            iss >> std::ws;
            if (iss.eof()) {
                polygons.push_back(std::move(curPolygon));
            }
        }
    }

    std::string stringCin;
    std::cout << std::fixed << std::setprecision(1);
    while (std::getline(std::cin, stringCin)) {
            if (stringCin.empty()) continue;

            std::istringstream lineOfCommand(stringCin);
            std::string mainCommand;
            lineOfCommand >> mainCommand;
            if (mainCommand == "AREA") {
                std::string subCommand;
                lineOfCommand >> subCommand;
                if (subCommand.empty()) {
                    std::cout << "<INVALID COMMAND>\n";
                        continue;
                }
                if (subCommand == "EVEN") {
                    if (lineOfCommand >> std::ws && !lineOfCommand.eof()) {
                        std::cout << "<INVALID COMMAND>\n";
                        continue;
                    }
                    double sum = std::accumulate(
                        polygons.begin(),
                        polygons.end(),
                        0.0,
                        SumAreaIfEven()
                    );
                    std::cout << sum << "\n";
                }
                else if (subCommand == "ODD") {
                    if (lineOfCommand >> std::ws && !lineOfCommand.eof()) {
                        std::cout << "<INVALID COMMAND>\n";
                        continue;
                    }
                    double sum = std::accumulate(
                        polygons.begin(),
                        polygons.end(),
                        0.0,
                        SumAreaIfOdd()
                    );
                    std::cout << sum << "\n";
                }
                else if (subCommand == "MEAN") {
                    if (lineOfCommand >> std::ws && !lineOfCommand.eof()) {
                        std::cout << "<INVALID COMMAND>\n";
                        continue;
                    }
                    if (polygons.empty()) {
                        std::cout << "<INVALID COMMAND>\n";
                    }
                    else {
                        double sum = std::accumulate(
                            polygons.begin(),
                            polygons.end(),
                            0.0,
                            SumArea()
                        );
                        std::cout << sum / polygons.size() << "\n";
                    }
                }
                else {
                    if (lineOfCommand >> std::ws && !lineOfCommand.eof()) {
                        std::cout << "<INVALID COMMAND>\n";
                        continue;
                    }
                    try {
                        std::size_t numOfFigures;
                        std::size_t numOfVertexes = std::stoul(subCommand, &numOfFigures);
                        if (numOfFigures != subCommand.size() || numOfVertexes < 3) {
                            std::cout << "<INVALID COMMAND>\n";
                        }
                        else {
                            double sum = std::accumulate(
                               polygons.begin(),
                               polygons.end(),
                              0.0,
                              SumAreaIfNumOfVertexes(numOfVertexes)
                            );
                            std::cout << sum << "\n";
                        }
                    }
                    catch (...) {
                        std::cout << "<INVALID COMMAND>\n";
                    }

                }
            }
            else if (mainCommand == "MAX") {
                std::string subCommand;
                lineOfCommand >> subCommand;
                if (subCommand == "AREA") {
                    if (lineOfCommand >> std::ws && !lineOfCommand.eof()) {
                        std::cout << "<INVALID COMMAND>\n";
                        continue;
                    }
                    if (polygons.empty()) {
                        std::cout << "<INVALID COMMAND>\n";
                    }
                    else {
                        auto maxElementIt = std::max_element(
                            polygons.begin(),
                            polygons.end(),
                            CompareArea()
                        );
                        std::cout << getArea(*maxElementIt) << "\n";
                    }
                }
                else if (subCommand == "VERTEXES") {
                    if (lineOfCommand >> std::ws && !lineOfCommand.eof()) {
                        std::cout << "<INVALID COMMAND>\n";
                        continue;
                    }
                    if (polygons.empty()) {
                        std::cout << "<INVALID COMMAND>\n";
                    }
                    else {
                        auto maxElementIt = std::max_element(
                            polygons.begin(),
                            polygons.end(),
                            CompareVertexes()
                        );
                        std::cout << maxElementIt->points.size() << "\n";
                    }
                }
                else {
                    std::cout << "<INVALID COMMAND>\n";
                }
            }
            else if (mainCommand == "MIN") {
                std::string subCommand;
                lineOfCommand >> subCommand;
                if (subCommand == "AREA") {
                    if (lineOfCommand >> std::ws && !lineOfCommand.eof()) {
                        std::cout << "<INVALID COMMAND>\n";
                        continue;
                    }
                    if (polygons.empty()) {
                        std::cout << "<INVALID COMMAND>\n";
                    }
                    else {
                        auto minElementIt = std::min_element(
                            polygons.begin(),
                            polygons.end(),
                            CompareArea()
                        );
                        std::cout << getArea(*minElementIt) << "\n";
                    }
                }
                else if (subCommand == "VERTEXES") {
                    if (lineOfCommand >> std::ws && !lineOfCommand.eof()) {
                        std::cout << "<INVALID COMMAND>\n";
                        continue;
                    }
                    if (polygons.empty()) {
                        std::cout << "<INVALID COMMAND>\n";
                    }
                    else {
                        auto minElementIt = std::min_element(
                            polygons.begin(),
                            polygons.end(),
                            CompareVertexes()
                        );
                        std::cout << minElementIt->points.size() << "\n";
                    }
                }
                else {
                    std::cout << "<INVALID COMMAND>\n";
                }
            }
            else if (mainCommand == "COUNT") {
                std::string subCommand;
                lineOfCommand >> subCommand;
                if (subCommand.empty()) {
                    std::cout << "<INVALID COMMAND>\n";
                        continue;
                }
                if (subCommand == "EVEN") {
                    if (lineOfCommand >> std::ws && !lineOfCommand.eof()) {
                        std::cout << "<INVALID COMMAND>\n";
                        continue;
                    }
                    std::size_t count = std::count_if(
                        polygons.begin(),
                        polygons.end(),
                        IsEven()
                    );
                    std::cout << count << "\n";
                }
                else if (subCommand == "ODD") {
                    if (lineOfCommand >> std::ws && !lineOfCommand.eof()) {
                        std::cout << "<INVALID COMMAND>\n";
                        continue;
                    }
                    std::size_t count = std::count_if(
                        polygons.begin(),
                        polygons.end(),
                        IsOdd()
                    );
                    std::cout << count << "\n";
                }
                else {
                    try {
                        if (lineOfCommand >> std::ws && !lineOfCommand.eof()) {
                            std::cout << "<INVALID COMMAND>\n";
                            continue;
                        }
                        std::size_t numOfFigures;
                        std::size_t numOfVertexes = std::stoul(subCommand, &numOfFigures);
                        if (numOfFigures != subCommand.size() || numOfVertexes < 3) {
                            std::cout << "<INVALID COMMAND>\n";
                            continue;
                        }
                        std::size_t count = std::count_if(
                            polygons.begin(),
                            polygons.end(),
                            IsNumOfVertexes(numOfVertexes)
                        );
                        std::cout << count << "\n";
                    }
                    catch (...) {
                        std::cout << "<INVALID COMMAND>\n";
                    }
                }
            }
            else if (mainCommand == "INFRAME") {
                if (polygons.empty()) {
                    std::cout << "<INVALID COMMAND>\n";
                    continue;
                }
                Polygon src;
                lineOfCommand >> src;
                if (!lineOfCommand || (lineOfCommand >> std::ws && !lineOfCommand.eof())) {
                    std::cout << "<INVALID COMMAND>\n";
                    continue;
                }

                Frame initial = getFrameOfPolygon(polygons[0]);
                Frame frame = std::accumulate(
                    polygons.begin() + 1,
                    polygons.end(),
                    initial,
                    MergeFrames()
                );

                bool isInside = std::all_of(
                        src.points.begin(),
                        src.points.end(),
                        IsInsideFrame(frame)
                );
                std::cout << (isInside ? "<TRUE>\n" : "<FALSE>\n");
            }
            else if (mainCommand == "INTERSECTIONS") {
                if (polygons.empty()) {
                    std::cout << "<INVALID COMMAND>\n";
                    continue;
                }
                Polygon src;
                lineOfCommand >> src;
                if (!lineOfCommand || (lineOfCommand >> std::ws && !lineOfCommand.eof())) {
                    std::cout << "<INVALID COMMAND>\n";
                    continue;
                }
                std::size_t count = std::count_if(
                    polygons.begin(),
                    polygons.end(),
                    IsIntersected(src)
                );
                std::cout << count << "\n";
            }
            else {
                std::cout << "<INVALID COMMAND>\n";
            }
    }
    return 0;
}
