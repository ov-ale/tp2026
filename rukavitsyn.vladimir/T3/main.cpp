#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>
#include <numeric>
#include <iterator>
#include <iomanip>
#include <stdexcept>

struct Point
{
    int x, y;
};

struct Polygon
{
    std::vector<Point> points;
};

bool operator==(const Point& a, const Point& b)
{
    return a.x == b.x && a.y == b.y;
}

bool operator==(const Polygon& a, const Polygon& b)
{
    return a.points == b.points;
}

std::istream& operator>>(std::istream& in, Point& p)
{
    char c;
    if (!(in >> c) || c != '(') { in.setstate(std::ios::failbit); return in; }
    if (!(in >> p.x)) return in;
    if (!(in >> c) || c != ';') { in.setstate(std::ios::failbit); return in; }
    if (!(in >> p.y)) return in;
    if (!(in >> c) || c != ')') { in.setstate(std::ios::failbit); return in; }
    return in;
}

std::istream& operator>>(std::istream& in, Polygon& poly)
{
    poly.points.clear();
    int n = 0;
    if (!(in >> n) || n < 3) { in.setstate(std::ios::failbit); return in; }

    poly.points.resize(n);
    std::for_each(poly.points.begin(), poly.points.end(),
        [&in](Point& p) { in >> p; });

    if (!in) { in.setstate(std::ios::failbit); }
    return in;
}

std::ostream& operator<<(std::ostream& out, const Point& p)
{
    return out << '(' << p.x << ';' << p.y << ')';
}

std::ostream& operator<<(std::ostream& out, const Polygon& poly)
{
    out << poly.points.size();
    std::for_each(poly.points.cbegin(), poly.points.cend(),
        [&out](const Point& p) { out << ' ' << p; });
    return out;
}

long long signedArea2(const Polygon& poly)
{
    const auto& pts = poly.points;
    long long sum = std::inner_product(
        pts.begin(), pts.end() - 1,
        pts.begin() + 1,
        0LL,
        std::plus<long long>(),
        [](const Point& a, const Point& b) -> long long {
            return static_cast<long long>(a.x) * b.y
                - static_cast<long long>(b.x) * a.y;
        });
    sum += static_cast<long long>(pts.back().x) * pts.front().y
        - static_cast<long long>(pts.front().x) * pts.back().y;
    return std::abs(sum);
}

double area(const Polygon& poly)
{
    return static_cast<double>(signedArea2(poly)) / 2.0;
}

long long dot(const Point& a, const Point& b, const Point& c)
{
    return static_cast<long long>(b.x - a.x) * (c.x - a.x)
        + static_cast<long long>(b.y - a.y) * (c.y - a.y);
}

bool hasRightAngle(const Polygon& poly)
{
    const auto& pts = poly.points;
    std::size_t n = pts.size();

    auto checkAngleAt = [&](std::size_t i) -> bool {
        const Point& prev = pts[(i + n - 1) % n];
        const Point& cur = pts[i];
        const Point& next = pts[(i + 1) % n];
        return dot(cur, prev, next) == 0;
        };

    std::vector<std::size_t> indices(n);
    std::iota(indices.begin(), indices.end(), 0);
    return std::any_of(indices.begin(), indices.end(), checkAngleAt);
}

bool isRect(const Polygon& poly)
{
    if (poly.points.size() != 4) return false;
    return hasRightAngle(poly);
}

bool isRectangle(const Polygon& poly)
{
    if (poly.points.size() != 4) return false;
    const auto& p = poly.points;
    for (std::size_t i = 0; i < 4; ++i)
    {
        if (dot(p[i], p[(i + 3) % 4], p[(i + 1) % 4]) != 0) return false;
    }
    return true;
}

struct PolygonEqual
{
    const Polygon& target;
    explicit PolygonEqual(const Polygon& t) : target(t) {}
    bool operator()(const Polygon& p) const { return p == target; }
};

void cmdArea(const std::vector<Polygon>& polygons, const std::string& param)
{
    if (polygons.empty() && param == "MEAN")
    {
        std::cout << "<INVALID COMMAND>\n";
        return;
    }

    if (param == "EVEN" || param == "ODD")
    {
        bool wantEven = (param == "EVEN");
        double sum = std::accumulate(polygons.begin(), polygons.end(), 0.0,
            [wantEven](double acc, const Polygon& poly) {
                bool isEven = (poly.points.size() % 2 == 0);
                return acc + (isEven == wantEven ? area(poly) : 0.0);
            });
        std::cout << std::fixed << std::setprecision(1) << sum << "\n";
    }
    else if (param == "MEAN")
    {
        double sum = std::accumulate(polygons.begin(), polygons.end(), 0.0,
            [](double acc, const Polygon& poly) { return acc + area(poly); });
        std::cout << std::fixed << std::setprecision(1)
            << sum / static_cast<double>(polygons.size()) << "\n";
    }
    else
    {
        std::size_t n = 0;
        try { n = std::stoul(param); }
        catch (...) { std::cout << "<INVALID COMMAND>\n"; return; }
        if (n < 3) { std::cout << "<INVALID COMMAND>\n"; return; }

        double sum = std::accumulate(polygons.begin(), polygons.end(), 0.0,
            [n](double acc, const Polygon& poly) {
                return acc + (poly.points.size() == n ? area(poly) : 0.0);
            });
        std::cout << std::fixed << std::setprecision(1) << sum << "\n";
    }
}

void cmdMax(const std::vector<Polygon>& polygons, const std::string& param)
{
    if (polygons.empty()) { std::cout << "<INVALID COMMAND>\n"; return; }

    if (param == "AREA")
    {
        auto it = std::max_element(polygons.begin(), polygons.end(),
            [](const Polygon& a, const Polygon& b) { return area(a) < area(b); });
        std::cout << std::fixed << std::setprecision(1) << area(*it) << "\n";
    }
    else if (param == "VERTEXES")
    {
        auto it = std::max_element(polygons.begin(), polygons.end(),
            [](const Polygon& a, const Polygon& b) {
                return a.points.size() < b.points.size();
            });
        std::cout << it->points.size() << "\n";
    }
    else
    {
        std::cout << "<INVALID COMMAND>\n";
    }
}

void cmdMin(const std::vector<Polygon>& polygons, const std::string& param)
{
    if (polygons.empty()) { std::cout << "<INVALID COMMAND>\n"; return; }

    if (param == "AREA")
    {
        auto it = std::min_element(polygons.begin(), polygons.end(),
            [](const Polygon& a, const Polygon& b) { return area(a) < area(b); });
        std::cout << std::fixed << std::setprecision(1) << area(*it) << "\n";
    }
    else if (param == "VERTEXES")
    {
        auto it = std::min_element(polygons.begin(), polygons.end(),
            [](const Polygon& a, const Polygon& b) {
                return a.points.size() < b.points.size();
            });
        std::cout << it->points.size() << "\n";
    }
    else
    {
        std::cout << "<INVALID COMMAND>\n";
    }
}

void cmdCount(const std::vector<Polygon>& polygons, const std::string& param)
{
    if (param == "EVEN" || param == "ODD")
    {
        bool wantEven = (param == "EVEN");
        long count = std::count_if(polygons.begin(), polygons.end(),
            [wantEven](const Polygon& p) {
                return (p.points.size() % 2 == 0) == wantEven;
            });
        std::cout << count << "\n";
    }
    else
    {
        std::size_t n = 0;
        try { n = std::stoul(param); }
        catch (...) { std::cout << "<INVALID COMMAND>\n"; return; }
        if (n < 3) { std::cout << "<INVALID COMMAND>\n"; return; }

        long count = std::count_if(polygons.begin(), polygons.end(),
            [n](const Polygon& p) { return p.points.size() == n; });
        std::cout << count << "\n";
    }
}

void cmdRects(const std::vector<Polygon>& polygons)
{
    long count = std::count_if(polygons.begin(), polygons.end(), isRectangle);
    std::cout << count << "\n";
}

void cmdMaxSeq(const std::vector<Polygon>& polygons, const Polygon& target)
{
    struct RunState { std::size_t cur; std::size_t max; };

    PolygonEqual eq(target);
    RunState state = std::accumulate(
        polygons.begin(), polygons.end(),
        RunState{ 0, 0 },
        [&eq](RunState st, const Polygon& poly) -> RunState {
            if (eq(poly))
            {
                ++st.cur;
                st.max = std::max(st.max, st.cur);
            }
            else
            {
                st.cur = 0;
            }
            return st;
        });

    std::cout << state.max << "\n";
}

bool parsePolygon(const std::string& rest, Polygon& out)
{
    std::istringstream ss(rest);
    ss >> out;
    return ss && !out.points.empty();
}


int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "Error: filename not specified.\n"
            << "Usage: " << argv[0] << " <filename>\n";
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file)
    {
        std::cerr << "Error: cannot open file \"" << argv[1] << "\"\n";
        return 1;
    }

    std::vector<Polygon> polygons;
    {
        std::string line;
        while (std::getline(file, line))
        {
            if (line.empty()) continue;
            std::istringstream ss(line);
            Polygon poly;
            if (ss >> poly)
                polygons.push_back(std::move(poly));
        }
    }

    std::string line;
    while (std::getline(std::cin, line))
    {
        std::istringstream ss(line);
        std::string cmd;
        if (!(ss >> cmd)) continue;

        if (cmd == "AREA")
        {
            std::string param;
            if (!(ss >> param)) { std::cout << "<INVALID COMMAND>\n"; continue; }
            cmdArea(polygons, param);
        }
        else if (cmd == "MAX")
        {
            std::string param;
            if (!(ss >> param)) { std::cout << "<INVALID COMMAND>\n"; continue; }
            cmdMax(polygons, param);
        }
        else if (cmd == "MIN")
        {
            std::string param;
            if (!(ss >> param)) { std::cout << "<INVALID COMMAND>\n"; continue; }
            cmdMin(polygons, param);
        }
        else if (cmd == "COUNT")
        {
            std::string param;
            if (!(ss >> param)) { std::cout << "<INVALID COMMAND>\n"; continue; }
            cmdCount(polygons, param);
        }
        else if (cmd == "RECTS")
        {
            std::string extra;
            if (ss >> extra) { std::cout << "<INVALID COMMAND>\n"; continue; }
            cmdRects(polygons);
        }
        else if (cmd == "MAXSEQ")
        {
            std::string rest;
            if (!std::getline(ss, rest) || rest.empty())
            {
                std::cout << "<INVALID COMMAND>\n"; continue;
            }
            Polygon target;
            if (!parsePolygon(rest, target))
            {
                std::cout << "<INVALID COMMAND>\n"; continue;
            }
            cmdMaxSeq(polygons, target);
        }
        else
        {
            std::cout << "<INVALID COMMAND>\n";
        }
    }

    return 0;
}
