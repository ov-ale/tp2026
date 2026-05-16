#include <iostream>
#include <functional>
#include <iterator>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>
#include <numeric>
#include <cstddef>
#include <limits>
#include <iomanip>
#include <cmath>
#include <set>

struct Point {
    int x, y;
    bool operator==(const Point& other) const { return x == other.x && y == other.y; }
    bool operator<(const Point& other) const { return x != other.x ? x < other.x : y < other.y; }
};

struct Polygon {
    std::vector<Point> points;
};

double computeArea(const Polygon& poly) {
    if (poly.points.size() < 3) return 0.0;
    double area = 0.0;
    size_t n = poly.points.size();
    for (size_t i = 0; i < n; ++i) {
        const Point& p1 = poly.points[i];
        const Point& p2 = poly.points[(i + 1) % n];
        area += static_cast<double>(p1.x * p2.y - p2.x * p1.y);
    }
    return std::abs(area) / 2.0;
}

struct BoundingBox {
    int minX, minY, maxX, maxY;
    bool valid;
    BoundingBox() : minX(0), minY(0), maxX(0), maxY(0), valid(false) {}
    void update(const Point& p) {
        if (!valid) {
            minX = maxX = p.x;
            minY = maxY = p.y;
            valid = true;
        }
        else {
            minX = std::min(minX, p.x);
            maxX = std::max(maxX, p.x);
            minY = std::min(minY, p.y);
            maxY = std::max(maxY, p.y);
        }
    }
    bool contains(const Point& p) const {
        return valid && p.x >= minX && p.x <= maxX && p.y >= minY && p.y <= maxY;
    }
};

BoundingBox getBoundingBox(const std::vector<Polygon>& polygons) {
    BoundingBox box;
    for (const auto& poly : polygons) {
        for (const auto& p : poly.points) {
            box.update(p);
        }
    }
    return box;
}

bool isInsideFrame(const Polygon& poly, const BoundingBox& frame) {
    if (!frame.valid) return false;
    for (const auto& p : poly.points) {
        if (!frame.contains(p)) return false;
    }
    return true;
}

std::set<Point> normalizePolygon(const Polygon& poly) {
    int minX = poly.points[0].x, minY = poly.points[0].y;
    for (const auto& p : poly.points) {
        minX = std::min(minX, p.x);
        minY = std::min(minY, p.y);
    }
    std::set<Point> normalized;
    for (const auto& p : poly.points) {
        normalized.insert({ p.x - minX, p.y - minY });
    }
    return normalized;
}

bool isPermutationOf(const Polygon& a, const Polygon& b) {
    if (a.points.size() != b.points.size()) return false;
    if (a.points.empty()) return true;
    return normalizePolygon(a) == normalizePolygon(b);
}

Polygon parsePolygon(const std::string& line) {
    std::istringstream iss(line);
    int vertexCount;
    iss >> vertexCount;
    if (vertexCount < 3) return Polygon{};

    Polygon poly;
    for (int i = 0; i < vertexCount; ++i) {
        char open, comma, close;
        int x, y;
        iss >> open >> x >> comma >> y >> close;
        if (open != '(' || comma != ',' || close != ')') {
            return Polygon{};
        }
        poly.points.push_back({ x, y });
    }

    if (poly.points.size() != static_cast<size_t>(vertexCount)) {
        return Polygon{};
    }
    return poly;
}

std::vector<Polygon> readPolygonsFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file");
    }

    std::vector<Polygon> polygons;
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        Polygon poly = parsePolygon(line);
        if (!poly.points.empty()) {
            polygons.push_back(poly);
        }
    }
    return polygons;
}

Polygon parsePolygonFromCommand(const std::string& cmd, size_t startPos) {
    std::string polyStr = cmd.substr(startPos);
    size_t firstNonSpace = polyStr.find_first_not_of(" \t");
    if (firstNonSpace != std::string::npos) {
        polyStr = polyStr.substr(firstNonSpace);
    }
    return parsePolygon(polyStr);
}

struct AreaCalculator {
    double operator()(const Polygon& p) const { return computeArea(p); }
};

struct HasVertexCount {
    int target;
    explicit HasVertexCount(int t) : target(t) {}
    bool operator()(const Polygon& p) const { return p.points.size() == static_cast<size_t>(target); }
};

struct HasVertexParity {
    bool even;
    explicit HasVertexParity(bool e) : even(e) {}
    bool operator()(const Polygon& p) const { return (p.points.size() % 2 == 0) == even; }
};

struct CompareByArea {
    bool operator()(const Polygon& a, const Polygon& b) const { return computeArea(a) < computeArea(b); }
};

struct CompareByVertexCount {
    bool operator()(const Polygon& a, const Polygon& b) const { return a.points.size() < b.points.size(); }
};

struct IsPermutation {
    Polygon target;
    explicit IsPermutation(const Polygon& t) : target(t) {}
    bool operator()(const Polygon& p) const { return isPermutationOf(target, p); }
};

void processArea(const std::vector<Polygon>& polygons, const std::string& param) {
    if (polygons.empty()) {
        std::cout << "<INVALID COMMAND>" << std::endl;
        return;
    }

    double sum = 0.0;
    if (param == "EVEN") {
        sum = std::accumulate(polygons.begin(), polygons.end(), 0.0,
            [](double acc, const Polygon& p) {
                return acc + (p.points.size() % 2 == 0 ? computeArea(p) : 0);
            });
    }
    else if (param == "ODD") {
        sum = std::accumulate(polygons.begin(), polygons.end(), 0.0,
            [](double acc, const Polygon& p) {
                return acc + (p.points.size() % 2 == 1 ? computeArea(p) : 0);
            });
    }
    else if (param == "MEAN") {
        sum = std::accumulate(polygons.begin(), polygons.end(), 0.0,
            std::bind(std::plus<double>(), std::placeholders::_1,
                std::bind(AreaCalculator(), std::placeholders::_2)));
        sum = sum / polygons.size();
    }
    else {
        int cnt = std::stoi(param);
        sum = std::accumulate(polygons.begin(), polygons.end(), 0.0,
            [cnt](double acc, const Polygon& p) {
                return acc + (p.points.size() == static_cast<size_t>(cnt) ? computeArea(p) : 0);
            });
    }
    std::cout << std::fixed << std::setprecision(1) << sum << std::endl;
}

void processCount(const std::vector<Polygon>& polygons, const std::string& param) {
    long long cnt = 0;
    if (param == "EVEN") {
        cnt = std::count_if(polygons.begin(), polygons.end(), HasVertexParity(true));
    }
    else if (param == "ODD") {
        cnt = std::count_if(polygons.begin(), polygons.end(), HasVertexParity(false));
    }
    else {
        cnt = std::count_if(polygons.begin(), polygons.end(), HasVertexCount(std::stoi(param)));
    }
    std::cout << cnt << std::endl;
}

void processMax(const std::vector<Polygon>& polygons, const std::string& param) {
    if (polygons.empty()) {
        std::cout << "<INVALID COMMAND>" << std::endl;
        return;
    }

    if (param == "AREA") {
        auto it = std::max_element(polygons.begin(), polygons.end(),
            std::bind(std::less<double>(), std::bind(AreaCalculator(), std::placeholders::_1),
                std::bind(AreaCalculator(), std::placeholders::_2)));
        std::cout << std::fixed << std::setprecision(1) << computeArea(*it) << std::endl;
    }
    else if (param == "VERTEXES") {
        auto it = std::max_element(polygons.begin(), polygons.end(), CompareByVertexCount());
        std::cout << it->points.size() << std::endl;
    }
    else {
        std::cout << "<INVALID COMMAND>" << std::endl;
    }
}

void processMin(const std::vector<Polygon>& polygons, const std::string& param) {
    if (polygons.empty()) {
        std::cout << "<INVALID COMMAND>" << std::endl;
        return;
    }

    if (param == "AREA") {
        auto it = std::min_element(polygons.begin(), polygons.end(), CompareByArea());
        std::cout << std::fixed << std::setprecision(1) << computeArea(*it) << std::endl;
    }
    else if (param == "VERTEXES") {
        auto it = std::min_element(polygons.begin(), polygons.end(), CompareByVertexCount());
        std::cout << it->points.size() << std::endl;
    }
    else {
        std::cout << "<INVALID COMMAND>" << std::endl;
    }
}

void processPerms(const std::vector<Polygon>& polygons, const std::string& cmd) {
    size_t spacePos = cmd.find(' ');
    if (spacePos == std::string::npos) {
        std::cout << "<INVALID COMMAND>" << std::endl;
        return;
    }

    Polygon target = parsePolygonFromCommand(cmd, spacePos + 1);
    if (target.points.empty()) {
        std::cout << "<INVALID COMMAND>" << std::endl;
        return;
    }

    long long cnt = std::count_if(polygons.begin(), polygons.end(), IsPermutation(target));
    std::cout << cnt << std::endl;
}

void processInFrame(const std::vector<Polygon>& polygons, const std::string& cmd) {
    if (polygons.empty()) {
        std::cout << "<INVALID COMMAND>" << std::endl;
        return;
    }

    size_t spacePos = cmd.find(' ');
    if (spacePos == std::string::npos) {
        std::cout << "<INVALID COMMAND>" << std::endl;
        return;
    }

    Polygon target = parsePolygonFromCommand(cmd, spacePos + 1);
    if (target.points.empty()) {
        std::cout << "<INVALID COMMAND>" << std::endl;
        return;
    }

    BoundingBox frame = getBoundingBox(polygons);
    bool result = isInsideFrame(target, frame);
    std::cout << (result ? "<TRUE>" : "<FALSE>") << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Error: filename parameter required" << std::endl;
        return 1;
    }

    std::vector<Polygon> polygons;
    try {
        polygons = readPolygonsFromFile(argv[1]);
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    std::string line;
    while (std::getline(std::cin, line)) {
        size_t start = line.find_first_not_of(" \t");
        if (start == std::string::npos) continue;
        size_t end = line.find_last_not_of(" \t");
        line = line.substr(start, end - start + 1);
        if (line.empty()) continue;

        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;

        if (cmd == "AREA") {
            std::string param;
            iss >> param;
            processArea(polygons, param);
        }
        else if (cmd == "COUNT") {
            std::string param;
            iss >> param;
            processCount(polygons, param);
        }
        else if (cmd == "MAX") {
            std::string param;
            iss >> param;
            processMax(polygons, param);
        }
        else if (cmd == "MIN") {
            std::string param;
            iss >> param;
            processMin(polygons, param);
        }
        else if (cmd == "PERMS") {
            processPerms(polygons, line);
        }
        else if (cmd == "INFRAME") {
            processInFrame(polygons, line);
        }
        else {
            std::cout << "<INVALID COMMAND>" << std::endl;
        }
    }

    return 0;
}
