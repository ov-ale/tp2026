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
#include <exception>
#include <cassert>

using namespace std::placeholders;

struct Point {
    int x, y;
};

struct Polygon {
    std::vector<Point> points;
};

std::istream& operator>>(std::istream& is, Point& p) {
    std::istream::sentry sentry(is);
    if (!is) return is;
    char c1, c2, c3;
    is >> c1 >> p.x >> c2 >> p.y >> c3;
    if (is && c1 == '(' && c2 == ';' && c3 == ')') {
        return is;
    }
    is.setstate(std::ios_base::failbit);
    return is;
}

bool operator==(const Point& p1, const Point& p2) {
    return p1.x == p2.x && p1.y == p2.y;
}

bool operator==(const Polygon& p1, const Polygon& p2) {
    if (p1.points.size() != p2.points.size()) return false;
    return p1.points == p2.points;
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

double calcAreaTerm(const Point& a, const Point& b) {
    return static_cast<double>(a.x * b.y - a.y * b.x);
}

double area(const Polygon& pg) {
    if (pg.points.size() < 3) return 0.0;
    std::vector<double> terms;
    std::transform(pg.points.begin(), pg.points.end() - 1, pg.points.begin() + 1,
        std::back_inserter(terms), calcAreaTerm);
    terms.push_back(calcAreaTerm(pg.points.back(), pg.points.front()));
    double sum = std::accumulate(terms.begin(), terms.end(), 0.0);
    return std::abs(sum) / 2.0;
}

bool isEven(const Polygon& pg) {
    return pg.points.size() % 2 == 0;
}

bool isOdd(const Polygon& pg) {
    return pg.points.size() % 2 == 1;
}

bool hasVertexCount(const Polygon& pg, size_t n) {
    return pg.points.size() == n;
}

double addAreaIfEven(double sum, const Polygon& p) {
    return isEven(p) ? sum + area(p) : sum;
}
double addAreaIfOdd(double sum, const Polygon& p) {
    return isOdd(p) ? sum + area(p) : sum;
}
double addAreaIfCount(double sum, const Polygon& p, size_t n) {
    return hasVertexCount(p, n) ? sum + area(p) : sum;
}
double addArea(double sum, const Polygon& p) {
    return sum + area(p);
}

bool compareArea(const Polygon& a, const Polygon& b) {
    return area(a) < area(b);
}
bool compareVertexCount(const Polygon& a, const Polygon& b) {
    return a.points.size() < b.points.size();
}

bool countEven(const Polygon& p) { return isEven(p); }
bool countOdd(const Polygon& p) { return isOdd(p); }
bool countByVertex(const Polygon& p, size_t n) { return hasVertexCount(p, n); }

struct RemoveDuplicate {
    const Polygon& target;
    explicit RemoveDuplicate(const Polygon& t) : target(t) {}

    std::reference_wrapper<std::vector<Polygon>> operator()(
        std::reference_wrapper<std::vector<Polygon>> res,
        const Polygon& curr) const {
        std::vector<Polygon>& vec = res.get();
        if (vec.empty()) {
            vec.push_back(curr);
        }
        else if (!(curr == target && vec.back() == target)) {
            vec.push_back(curr);
        }
        return res;
    }
};

std::vector<Polygon> removeConsecutiveDuplicates(const std::vector<Polygon>& polys, const Polygon& target) {
    if (polys.empty()) return polys;

    std::vector<Polygon> result;
    result.push_back(polys[0]);

    std::accumulate(polys.begin() + 1, polys.end(), std::ref(result), RemoveDuplicate(target));

    return result;
}


struct BoundingBox {
    int minX, maxX, minY, maxY;
    BoundingBox() : minX(std::numeric_limits<int>::max()),
        maxX(std::numeric_limits<int>::min()),
        minY(std::numeric_limits<int>::max()),
        maxY(std::numeric_limits<int>::min()) {
    }
};

struct UpdateBox {
    BoundingBox operator()(BoundingBox box, const Point& p) const {
        if (p.x < box.minX) box.minX = p.x;
        if (p.x > box.maxX) box.maxX = p.x;
        if (p.y < box.minY) box.minY = p.y;
        if (p.y > box.maxY) box.maxY = p.y;
        return box;
    }
};

BoundingBox getBoundingBox(const Polygon& pg) {
    if (pg.points.empty()) return BoundingBox();
    BoundingBox box;
    box = std::accumulate(pg.points.begin(), pg.points.end(), box, UpdateBox());
    return box;
}

bool pointInBox(const Point& p, const BoundingBox& box) {
    return p.x >= box.minX && p.x <= box.maxX &&
        p.y >= box.minY && p.y <= box.maxY;
}

bool polygonInBox(const Polygon& pg, const BoundingBox& box) {
    return std::all_of(pg.points.begin(), pg.points.end(),
        std::bind(pointInBox, _1, box));
}

BoundingBox mergeBoxes(const BoundingBox& a, const BoundingBox& b) {
    BoundingBox result;
    result.minX = std::min(a.minX, b.minX);
    result.maxX = std::max(a.maxX, b.maxX);
    result.minY = std::min(a.minY, b.minY);
    result.maxY = std::max(a.maxY, b.maxY);
    return result;
}

struct MergeBoxes {
    BoundingBox operator()(BoundingBox box, const Polygon& poly) const {
        return mergeBoxes(box, getBoundingBox(poly));
    }
};

BoundingBox getGlobalBoundingBox(const std::vector<Polygon>& polys) {
    BoundingBox global = getBoundingBox(polys[0]);
    global = std::accumulate(polys.begin() + 1, polys.end(), global, MergeBoxes());
    return global;
}

struct CountRemovedDuplicates {
    const Polygon& target;
    explicit CountRemovedDuplicates(const Polygon& t) : target(t) {}

    int operator()(int count, const std::pair<const Polygon&, const Polygon&>& pair) const {
        return count + (pair.first == target && pair.second == target ? 1 : 0);
    }
};

struct MakePair {
    std::pair<const Polygon&, const Polygon&> operator()(const Polygon& a, const Polygon& b) const {
        return { a, b };
    }
};

int countConsecutiveDuplicates(const std::vector<Polygon>& polys, const Polygon& target) {
    if (polys.size() < 2) return 0;

    std::vector<std::pair<const Polygon&, const Polygon&>> pairs;
    pairs.reserve(polys.size() - 1);

    std::transform(polys.begin(), polys.end() - 1, polys.begin() + 1,
        std::back_inserter(pairs), MakePair());

    return std::accumulate(pairs.begin(), pairs.end(), 0, CountRemovedDuplicates(target));
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>\n";
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file) {
        std::cerr << "Error opening file\n";
        return 1;
    }

    std::vector<Polygon> polys;
    while (file) {
        Polygon p;
        if (file >> p) {
            polys.push_back(p);
        }
        else if (!file.eof()) {
            file.clear();
            file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    std::string command;
    while (std::cin >> command) {
        if (command == "AREA") {
            std::string mode;
            if (!(std::cin >> mode)) {
                std::cout << "<INVALID COMMAND>\n";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }

            if (std::cin.peek() != '\n' && (std::cin >> std::ws, !std::cin.eof())) {
                std::cout << "<INVALID COMMAND>\n";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }

            double result = 0.0;

            if (mode == "EVEN") {
                result = std::accumulate(polys.begin(), polys.end(), 0.0, addAreaIfEven);
            }
            else if (mode == "ODD") {
                result = std::accumulate(polys.begin(), polys.end(), 0.0, addAreaIfOdd);
            }
            else if (mode == "MEAN") {
                if (polys.empty()) {
                    std::cout << "<INVALID COMMAND>\n";
                    continue;
                }
                result = std::accumulate(polys.begin(), polys.end(), 0.0, addArea) / polys.size();
            }
            else {
                try {
                    size_t vertexCount = std::stoull(mode);
                    if (vertexCount < 3) throw std::invalid_argument("vertexCount < 3");
                    result = std::accumulate(polys.begin(), polys.end(), 0.0,
                        std::bind(addAreaIfCount, _1, _2, vertexCount));
                }
                catch (...) {
                    std::cout << "<INVALID COMMAND>\n";
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }
            }
            std::cout << std::fixed << std::setprecision(1) << result << "\n";
        }

        else if (command == "MIN") {
            std::string mode;
            if (!(std::cin >> mode)) {
                std::cout << "<INVALID COMMAND>\n";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }

            if (polys.empty()) {
                std::cout << "<INVALID COMMAND>\n";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }

            if (std::cin.peek() != '\n' && (std::cin >> std::ws, !std::cin.eof())) {
                std::cout << "<INVALID COMMAND>\n";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }

            if (mode == "AREA") {
                std::cout << std::fixed << std::setprecision(1)
                    << area(*std::min_element(polys.begin(), polys.end(), compareArea)) << "\n";
            }
            else if (mode == "VERTEXES") {
                std::cout << std::min_element(polys.begin(), polys.end(), compareVertexCount)->points.size() << "\n";
            }
            else {
                std::cout << "<INVALID COMMAND>\n";
            }
        }

        else if (command == "MAX") {
            std::string mode;
            if (!(std::cin >> mode)) {
                std::cout << "<INVALID COMMAND>\n";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }

            if (polys.empty()) {
                std::cout << "<INVALID COMMAND>\n";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }

            if (std::cin.peek() != '\n' && (std::cin >> std::ws, !std::cin.eof())) {
                std::cout << "<INVALID COMMAND>\n";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }

            if (mode == "AREA") {
                std::cout << std::fixed << std::setprecision(1)
                    << area(*std::max_element(polys.begin(), polys.end(), compareArea)) << "\n";
            }
            else if (mode == "VERTEXES") {
                std::cout << std::max_element(polys.begin(), polys.end(), compareVertexCount)->points.size() << "\n";
            }
            else {
                std::cout << "<INVALID COMMAND>\n";
            }
        }

        else if (command == "COUNT") {
            std::string mode;
            if (!(std::cin >> mode)) {
                std::cout << "<INVALID COMMAND>\n";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }

            if (std::cin.peek() != '\n' && (std::cin >> std::ws, !std::cin.eof())) {
                std::cout << "<INVALID COMMAND>\n";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }

            if (mode == "EVEN") {
                std::cout << std::count_if(polys.begin(), polys.end(), countEven) << "\n";
            }
            else if (mode == "ODD") {
                std::cout << std::count_if(polys.begin(), polys.end(), countOdd) << "\n";
            }
            else {
                try {
                    size_t vertexCount = std::stoull(mode);
                    if (vertexCount < 3) throw std::invalid_argument("vertexCount < 3");
                    std::cout << std::count_if(polys.begin(), polys.end(),
                        std::bind(countByVertex, _1, vertexCount)) << "\n";
                }
                catch (...) {
                    std::cout << "<INVALID COMMAND>\n";
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }
            }
        }

        else if (command == "RMECHO") {
            Polygon target;
            if (!(std::cin >> target)) {
                std::cout << "<INVALID COMMAND>\n";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }

            if (std::cin.peek() != '\n' && (std::cin >> std::ws, !std::cin.eof())) {
                std::cout << "<INVALID COMMAND>\n";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }

            int removed = countConsecutiveDuplicates(polys, target);
            polys = removeConsecutiveDuplicates(polys, target);
            std::cout << removed << "\n";
        }

        else if (command == "INFRAME") {
            Polygon target;
            if (!(std::cin >> target)) {
                std::cout << "<INVALID COMMAND>\n";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }

            if (std::cin.peek() != '\n' && (std::cin >> std::ws, !std::cin.eof())) {
                std::cout << "<INVALID COMMAND>\n";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }

            if (polys.empty()) {
                std::cout << "<INVALID COMMAND>\n";
                continue;
            }

            BoundingBox globalBox = getGlobalBoundingBox(polys);
            if (polygonInBox(target, globalBox)) {
                std::cout << "<TRUE>\n";
            }
            else {
                std::cout << "<FALSE>\n";
            }
        }

        else {
            std::cout << "<INVALID COMMAND>\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    return 0;
}
