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
    if (!(in >> p.x))            return in;
    if (!(in >> c) || c != ';') { in.setstate(std::ios::failbit); return in; }
    if (!(in >> p.y))            return in;
    if (!(in >> c) || c != ')') { in.setstate(std::ios::failbit); return in; }
    return in;
}

std::istream& operator>>(std::istream& in, Polygon& poly)
{
    poly.points.clear();
    int n = 0;
    if (!(in >> n) || n < 3) { in.setstate(std::ios::failbit); return in; }

    poly.points.resize(n);
    std::copy_n(std::istream_iterator<Point>(in), n, poly.points.begin());

    if (!in) { in.setstate(std::ios::failbit); return in; }

    std::string extra;
    if (in >> extra) { in.setstate(std::ios::failbit); return in; }
    in.clear();
    return in;
}

std::ostream& operator<<(std::ostream& out, const Point& p)
{
    return out << '(' << p.x << ';' << p.y << ')';
}

std::ostream& operator<<(std::ostream& out, const Polygon& poly)
{
    out << poly.points.size();
    std::copy(poly.points.cbegin(), poly.points.cend(),
        std::ostream_iterator<Point>(out, " "));
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

struct AreaAccum
{
    bool wantEven;
    double operator()(double acc, const Polygon& poly) const
    {
        bool isEven = (poly.points.size() % 2 == 0);
        return acc + (isEven == wantEven ? area(poly) : 0.0);
    }
};

struct AreaAccumN
{
    std::size_t n;
    double operator()(double acc, const Polygon& poly) const
    {
        return acc + (poly.points.size() == n ? area(poly) : 0.0);
    }
};

struct AreaLess
{
    bool operator()(const Polygon& a, const Polygon& b) const
    {
        return area(a) < area(b);
    }
};

struct SizeLess
{
    bool operator()(const Polygon& a, const Polygon& b) const
    {
        return a.points.size() < b.points.size();
    }
};

struct IsEvenOdd
{
    bool wantEven;
    bool operator()(const Polygon& p) const
    {
        return (p.points.size() % 2 == 0) == wantEven;
    }
};

struct HasNVertexes
{
    std::size_t n;
    bool operator()(const Polygon& p) const { return p.points.size() == n; }
};

struct PolygonEqual
{
    const Polygon& target;
    explicit PolygonEqual(const Polygon& t) : target(t) {}
    bool operator()(const Polygon& p) const { return p == target; }
};

long long edgeDot(const Point& a, const Point& b, const Point& c)
{
    return static_cast<long long>(b.x - a.x) * (c.x - b.x)
         + static_cast<long long>(b.y - a.y) * (c.y - b.y);
}

struct IsRightAt
{
    const Polygon& poly;
    explicit IsRightAt(const Polygon& p) : poly(p) {}
    bool operator()(std::size_t i) const
    {
        const auto& pts = poly.points;
        std::size_t n   = pts.size();
        return edgeDot(pts[i], pts[(i + 1) % n], pts[(i + 2) % n]) == 0;
    }
};

bool isRectangle(const Polygon& poly)
{
    if (poly.points.size() != 4) return false;
    std::vector<std::size_t> idx = {0, 1, 2, 3};
    return std::all_of(idx.begin(), idx.end(), IsRightAt(poly));
}

struct RunState
{
    std::size_t cur;
    std::size_t max;
};

struct MaxSeqAccum
{
    PolygonEqual eq;
    explicit MaxSeqAccum(const Polygon& t) : eq(t) {}
    RunState operator()(RunState st, const Polygon& poly) const
    {
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
    }
};

void cmdArea(const std::vector<Polygon>& polygons, const std::string& param)
{
    if (param == "EVEN" || param == "ODD")
    {
        double sum = std::accumulate(polygons.begin(), polygons.end(), 0.0,
            AreaAccum{param == "EVEN"});
        std::cout << std::fixed << std::setprecision(1) << sum << "\n";
    }
    else if (param == "MEAN")
    {
        if (polygons.empty()) { std::cout << "<INVALID COMMAND>\n"; return; }
        double sum = std::accumulate(polygons.begin(), polygons.end(), 0.0,
            [](double acc, const Polygon& poly) { return acc + area(poly); });
        std::cout << std::fixed << std::setprecision(1)
                  << sum / static_cast<double>(polygons.size()) << "\n";
    }
    else
    {
        std::size_t n = 0;
        try { n = std::stoul(param); } catch (...) { std::cout << "<INVALID COMMAND>\n"; return; }
        if (n < 3) { std::cout << "<INVALID COMMAND>\n"; return; }
        double sum = std::accumulate(polygons.begin(), polygons.end(), 0.0,
            AreaAccumN{n});
        std::cout << std::fixed << std::setprecision(1) << sum << "\n";
    }
}

void cmdMax(const std::vector<Polygon>& polygons, const std::string& param)
{
    if (polygons.empty()) { std::cout << "<INVALID COMMAND>\n"; return; }
    if (param == "AREA")
    {
        auto it = std::max_element(polygons.begin(), polygons.end(), AreaLess{});
        std::cout << std::fixed << std::setprecision(1) << area(*it) << "\n";
    }
    else if (param == "VERTEXES")
    {
        auto it = std::max_element(polygons.begin(), polygons.end(), SizeLess{});
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
        auto it = std::min_element(polygons.begin(), polygons.end(), AreaLess{});
        std::cout << std::fixed << std::setprecision(1) << area(*it) << "\n";
    }
    else if (param == "VERTEXES")
    {
        auto it = std::min_element(polygons.begin(), polygons.end(), SizeLess{});
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
        std::cout << std::count_if(polygons.begin(), polygons.end(),
            IsEvenOdd{param == "EVEN"}) << "\n";
    }
    else
    {
        std::size_t n = 0;
        try { n = std::stoul(param); } catch (...) { std::cout << "<INVALID COMMAND>\n"; return; }
        if (n < 3) { std::cout << "<INVALID COMMAND>\n"; return; }
        std::cout << std::count_if(polygons.begin(), polygons.end(),
            HasNVertexes{n}) << "\n";
    }
}

void cmdRects(const std::vector<Polygon>& polygons)
{
    std::cout << std::count_if(polygons.begin(), polygons.end(), isRectangle) << "\n";
}

void cmdMaxSeq(const std::vector<Polygon>& polygons, const Polygon& target)
{
    RunState state = std::accumulate(polygons.begin(), polygons.end(),
        RunState{0, 0}, MaxSeqAccum{target});
    std::cout << state.max << "\n";
}

bool parsePolygon(const std::string& rest, Polygon& out)
{
    std::istringstream ss(rest);
    ss >> out;
    return ss && !out.points.empty();
}

bool hasExtraTokens(std::istringstream& ss)
{
    std::string extra;
    return static_cast<bool>(ss >> extra);
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "Error: filename not specified.\nUsage: " << argv[0] << " <filename>\n";
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
            if (!(ss >> param) || hasExtraTokens(ss)) { std::cout << "<INVALID COMMAND>\n"; continue; }
            cmdArea(polygons, param);
        }
        else if (cmd == "MAX")
        {
            std::string param;
            if (!(ss >> param) || hasExtraTokens(ss)) { std::cout << "<INVALID COMMAND>\n"; continue; }
            cmdMax(polygons, param);
        }
        else if (cmd == "MIN")
        {
            std::string param;
            if (!(ss >> param) || hasExtraTokens(ss)) { std::cout << "<INVALID COMMAND>\n"; continue; }
            cmdMin(polygons, param);
        }
        else if (cmd == "COUNT")
        {
            std::string param;
            if (!(ss >> param) || hasExtraTokens(ss)) { std::cout << "<INVALID COMMAND>\n"; continue; }
            cmdCount(polygons, param);
        }
        else if (cmd == "RECTS")
        {
            if (hasExtraTokens(ss)) { std::cout << "<INVALID COMMAND>\n"; continue; }
            cmdRects(polygons);
        }
        else if (cmd == "MAXSEQ")
        {
            std::string rest;
            if (!std::getline(ss, rest) || rest.empty()) { std::cout << "<INVALID COMMAND>\n"; continue; }
            Polygon target;
            if (!parsePolygon(rest, target))             { std::cout << "<INVALID COMMAND>\n"; continue; }
            cmdMaxSeq(polygons, target);
        }
        else
        {
            std::cout << "<INVALID COMMAND>\n";
        }
    }

    return 0;
}
