#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <iterator>
#include <string>
#include <sstream>
#include <cmath>
#include <iomanip>
#include <cctype>

struct Point {
    int x, y;
    Point(int x = 0, int y = 0) : x(x), y(y) {}
    bool operator==(const Point& other) const { return x == other.x && y == other.y; }
    bool operator!=(const Point& other) const { return !(*this == other); }
    bool operator<(const Point& other) const {
        if (x != other.x) return x < other.x;
        return y < other.y;
    }
};

std::ostream& operator<<(std::ostream& out, const Point& p) {
    out << "(" << p.x << ";" << p.y << ")";
    return out;
}

struct Polygon {
    std::vector<Point> points;
    bool operator==(const Polygon& other) const {
        if (points.size() != other.points.size()) return false;
        for (size_t i = 0; i < points.size(); ++i)
            if (points[i] != other.points[i]) return false;
        return true;
    }

    bool operator!=(const Polygon& other) const {
        return !(*this == other);
    }

    size_t getVertexCount() const { return points.size(); }
    double getArea() const {
        if (points.size() < 3) return 0.0;
        double area = 0.0;
        for (size_t i = 0; i < points.size(); ++i) {
            const Point& p1 = points[i];
            const Point& p2 = points[(i + 1) % points.size()];
            area += p1.x * p2.y - p2.x * p1.y;
        }
        return std::abs(area) / 2.0;
    }
    Point getMinPoint() const {
        if (points.empty()) return Point(0, 0);
        auto it = std::min_element(points.begin(), points.end(),
            [](const Point& a, const Point& b) {
                if (a.x != b.x) return a.x < b.x;
                return a.y < b.y;
            });
        return *it;
    }
    Point getMaxPoint() const {
        if (points.empty()) return Point(0, 0);
        auto it = std::max_element(points.begin(), points.end(),
            [](const Point& a, const Point& b) {
                if (a.x != b.x) return a.x < b.x;
                return a.y < b.y;
            });
        return *it;
    }
    bool isInsideBoundingBox(const Point& minBound, const Point& maxBound) const {
        Point minP = getMinPoint();
        Point maxP = getMaxPoint();
        return minP.x >= minBound.x && minP.y >= minBound.y &&
            maxP.x <= maxBound.x && maxP.y <= maxBound.y;
    }
};

std::ostream& operator<<(std::ostream& out, const Polygon& poly) {
    out << poly.points.size();
    for (const auto& p : poly.points) out << " " << p;
    return out;
}

std::istream& operator>>(std::istream& in, Polygon& poly) {
    std::string line;
    if (!std::getline(in, line)) return in;
    if (line.empty()) return in;
    std::istringstream iss(line);
    int vertexCount;
    if (!(iss >> vertexCount) || vertexCount < 3) {
        in.setstate(std::ios::failbit);
        return in;
    }
    std::vector<Point> points;
    for (int i = 0; i < vertexCount; ++i) {
        std::string token;
        if (!(iss >> token)) {
            in.setstate(std::ios::failbit);
            return in;
        }
        if (token.empty() || token.front() != '(' || token.back() != ')') {
            in.setstate(std::ios::failbit);
            return in;
        }
        std::string inner = token.substr(1, token.length() - 2);
        size_t semicolonPos = inner.find(';');
        if (semicolonPos == std::string::npos) {
            in.setstate(std::ios::failbit);
            return in;
        }
        try {
            Point p;
            p.x = std::stoi(inner.substr(0, semicolonPos));
            p.y = std::stoi(inner.substr(semicolonPos + 1));
            points.push_back(p);
        }
        catch (...) {
            in.setstate(std::ios::failbit);
            return in;
        }
    }
    std::string extra;
    if (iss >> extra) {
        in.setstate(std::ios::failbit);
        return in;
    }
    poly.points = points;
    return in;
}

bool parsePolygonFromArgs(const std::vector<std::string>& args, Polygon& poly) {
    if (args.empty()) return false;
    int vertexCount;
    try { vertexCount = std::stoi(args[0]); }
    catch (...) { return false; }
    if (vertexCount < 3 || args.size() < 1 + static_cast<size_t>(vertexCount)) return false;
    std::vector<Point> points;
    for (int i = 0; i < vertexCount; ++i) {
        const std::string& token = args[1 + i];
        if (token.empty() || token.front() != '(' || token.back() != ')') return false;
        std::string inner = token.substr(1, token.length() - 2);
        size_t semicolonPos = inner.find(';');
        if (semicolonPos == std::string::npos) return false;
        try {
            Point p;
            p.x = std::stoi(inner.substr(0, semicolonPos));
            p.y = std::stoi(inner.substr(semicolonPos + 1));
            points.push_back(p);
        }
        catch (...) { return false; }
    }
    poly.points = points;
    return true;
}

struct CompareArea {
    bool operator()(const Polygon& a, const Polygon& b) const {
        return a.getArea() < b.getArea();
    }
};

struct CompareVertexCount {
    bool operator()(const Polygon& a, const Polygon& b) const {
        return a.getVertexCount() < b.getVertexCount();
    }
};

struct CompareMinPoint {
    bool operator()(const Polygon& a, const Polygon& b) const {
        Point minA = a.getMinPoint();
        Point minB = b.getMinPoint();
        if (minA.x != minB.x) return minA.x < minB.x;
        return minA.y < minB.y;
    }
};

struct CompareMaxPoint {
    bool operator()(const Polygon& a, const Polygon& b) const {
        Point maxA = a.getMaxPoint();
        Point maxB = b.getMaxPoint();
        if (maxA.x != maxB.x) return maxA.x < maxB.x;
        return maxA.y < maxB.y;
    }
};

struct Identity {
    Polygon operator()(const Polygon& p) const { return p; }
};

struct IsTarget {
    const Polygon& target_;
    IsTarget(const Polygon& target) : target_(target) {}
    bool operator()(const Polygon& p) const { return p == target_; }
};

struct IsNotTarget {
    const Polygon& target_;
    IsNotTarget(const Polygon& target) : target_(target) {}
    bool operator()(const Polygon& p) const { return p != target_; }
};

struct IsDuplicateAfterRemoval {
    const Polygon& target_;
    bool firstFound_;
    IsDuplicateAfterRemoval(const Polygon& target) : target_(target), firstFound_(false) {}
    bool operator()(const Polygon& p) {
        if (p == target_) {
            if (firstFound_) return true;
            firstFound_ = true;
            return false;
        }
        firstFound_ = false;
        return false;
    }
};

template<bool Odd>
struct SumAreaByParity {
    double operator()(double sum, const Polygon& poly) const {
        bool isOdd = (poly.getVertexCount() % 2 == 1);
        return sum + (isOdd == Odd ? poly.getArea() : 0.0);
    }
};

struct SumAreaByVertexCount {
    int target_;
    SumAreaByVertexCount(int target) : target_(target) {}
    double operator()(double sum, const Polygon& poly) const {
        return sum + (static_cast<int>(poly.getVertexCount()) == target_ ? poly.getArea() : 0.0);
    }
};

struct SumArea {
    double operator()(double sum, const Polygon& poly) const {
        return sum + poly.getArea();
    }
};

template<bool Odd>
struct CountByParity {
    bool operator()(const Polygon& poly) const {
        return (poly.getVertexCount() % 2 == 1) == Odd;
    }
};

struct CountByVertexCount {
    int target_;
    CountByVertexCount(int target) : target_(target) {}
    bool operator()(const Polygon& poly) const {
        return static_cast<int>(poly.getVertexCount()) == target_;
    }
};

class InputStreamIterator {
public:
    using iterator_category = std::input_iterator_tag;
    using value_type = Polygon;
    using difference_type = std::ptrdiff_t;
    using pointer = const Polygon*;
    using reference = const Polygon&;

    InputStreamIterator() : in_(nullptr) {}
    explicit InputStreamIterator(std::istream& in) : in_(&in) { ++(*this); }

    reference operator*() const { return value_; }
    pointer operator->() const { return &value_; }

    InputStreamIterator& operator++() {
        if (!in_) return *this;
        std::string line;
        while (std::getline(*in_, line)) {
            if (line.empty()) continue;
            std::istringstream iss(line);
            Polygon poly;
            if (iss >> poly) {
                value_ = poly;
                return *this;
            }
        }
        in_ = nullptr;
        return *this;
    }

    InputStreamIterator operator++(int) {
        InputStreamIterator tmp = *this;
        ++(*this);
        return tmp;
    }

    bool operator==(const InputStreamIterator& other) const { return in_ == other.in_; }
    bool operator!=(const InputStreamIterator& other) const { return !(*this == other); }

private:
    std::istream* in_;
    Polygon value_;
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Error: filename not specified" << std::endl;
        return 1;
    }
    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Error: cannot open file " << argv[1] << std::endl;
        return 1;
    }

    std::vector<Polygon> polygons;
    InputStreamIterator inputBegin(file);
    InputStreamIterator inputEnd;
    std::copy(inputBegin, inputEnd, std::back_inserter(polygons));
    file.close();

    std::string cmdLine;
    while (std::getline(std::cin, cmdLine)) {
        if (cmdLine.empty()) continue;
        std::istringstream iss(cmdLine);
        std::string command;
        iss >> command;

        try {
            if (command == "AREA") {
                std::string param;
                iss >> param;
                if (param == "EVEN") {
                    double sum = std::accumulate(polygons.begin(), polygons.end(), 0.0, SumAreaByParity<false>());
                    std::cout << std::fixed << std::setprecision(1) << sum << std::endl;
                }
                else if (param == "ODD") {
                    double sum = std::accumulate(polygons.begin(), polygons.end(), 0.0, SumAreaByParity<true>());
                    std::cout << std::fixed << std::setprecision(1) << sum << std::endl;
                }
                else if (param == "MEAN") {
                    if (polygons.empty()) throw std::runtime_error("No shapes");
                    double sum = std::accumulate(polygons.begin(), polygons.end(), 0.0, SumArea());
                    double mean = sum / polygons.size();
                    std::cout << std::fixed << std::setprecision(1) << mean << std::endl;
                }
                else {
                    int vertexCount = std::stoi(param);
                    double sum = std::accumulate(polygons.begin(), polygons.end(), 0.0, SumAreaByVertexCount(vertexCount));
                    std::cout << std::fixed << std::setprecision(1) << sum << std::endl;
                }
            }
            else if (command == "MAX") {
                std::string param;
                iss >> param;
                if (param == "AREA") {
                    if (polygons.empty()) throw std::runtime_error("No shapes");
                    auto it = std::max_element(polygons.begin(), polygons.end(), CompareArea());
                    std::cout << std::fixed << std::setprecision(1) << it->getArea() << std::endl;
                }
                else if (param == "VERTEXES") {
                    if (polygons.empty()) throw std::runtime_error("No shapes");
                    auto it = std::max_element(polygons.begin(), polygons.end(), CompareVertexCount());
                    std::cout << it->getVertexCount() << std::endl;
                }
                else {
                    std::cout << "<INVALID COMMAND>" << std::endl;
                }
            }
            else if (command == "MIN") {
                std::string param;
                iss >> param;
                if (param == "AREA") {
                    if (polygons.empty()) throw std::runtime_error("No shapes");
                    auto it = std::min_element(polygons.begin(), polygons.end(), CompareArea());
                    std::cout << std::fixed << std::setprecision(1) << it->getArea() << std::endl;
                }
                else if (param == "VERTEXES") {
                    if (polygons.empty()) throw std::runtime_error("No shapes");
                    auto it = std::min_element(polygons.begin(), polygons.end(), CompareVertexCount());
                    std::cout << it->getVertexCount() << std::endl;
                }
                else {
                    std::cout << "<INVALID COMMAND>" << std::endl;
                }
            }
            else if (command == "COUNT") {
                std::string param;
                iss >> param;
                if (param == "EVEN") {
                    size_t cnt = std::count_if(polygons.begin(), polygons.end(), CountByParity<false>());
                    std::cout << cnt << std::endl;
                }
                else if (param == "ODD") {
                    size_t cnt = std::count_if(polygons.begin(), polygons.end(), CountByParity<true>());
                    std::cout << cnt << std::endl;
                }
                else {
                    int vertexCount = std::stoi(param);
                    size_t cnt = std::count_if(polygons.begin(), polygons.end(), CountByVertexCount(vertexCount));
                    std::cout << cnt << std::endl;
                }
            }
            else if (command == "RMECHO") {
                std::vector<std::string> args;
                std::string arg;
                while (iss >> arg) args.push_back(arg);
                if (args.empty()) {
                    std::cout << "<INVALID COMMAND>" << std::endl;
                    continue;
                }
                Polygon target;
                if (!parsePolygonFromArgs(args, target)) {
                    std::cout << "<INVALID COMMAND>" << std::endl;
                    continue;
                }
                std::vector<Polygon> temp;
                std::unique_copy(polygons.begin(), polygons.end(), std::back_inserter(temp),
                    [&target](const Polygon& a, const Polygon& b) {
                        return a == target && b == target;
                    });
                int removed = polygons.size() - temp.size();
                polygons = temp;
                std::cout << removed << std::endl;
            }
            else if (command == "INFRAME") {
                std::vector<std::string> args;
                std::string arg;
                while (iss >> arg) args.push_back(arg);
                if (args.empty()) {
                    std::cout << "<INVALID COMMAND>" << std::endl;
                    continue;
                }
                Polygon target;
                if (!parsePolygonFromArgs(args, target)) {
                    std::cout << "<INVALID COMMAND>" << std::endl;
                    continue;
                }
                if (polygons.empty()) {
                    std::cout << "<TRUE>" << std::endl;
                    continue;
                }
                auto minIt = std::min_element(polygons.begin(), polygons.end(), CompareMinPoint());
                auto maxIt = std::max_element(polygons.begin(), polygons.end(), CompareMaxPoint());
                Point globalMin = minIt->getMinPoint();
                Point globalMax = maxIt->getMaxPoint();
                bool result = target.isInsideBoundingBox(globalMin, globalMax);
                std::cout << (result ? "<TRUE>" : "<FALSE>") << std::endl;
            }
            else {
                std::cout << "<INVALID COMMAND>" << std::endl;
            }
        }
        catch (const std::exception&) {
            std::cout << "<INVALID COMMAND>" << std::endl;
        }
    }
    return 0;
}
