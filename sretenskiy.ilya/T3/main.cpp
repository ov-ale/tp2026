#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <functional>
#include <cmath>
#include <stdexcept>
#include <limits>
#include <iterator>
#include <sstream>

struct Point
{
    int x, y;
};

struct Polygon
{
    std::vector<Point> points;
};

std::istream& operator>>(std::istream& is, Point& p)
{
    char ch1, ch2, ch3;
    if (is >> ch1 >> p.x >> ch2 >> p.y >> ch3)
    {
        if (ch1 == '(' && ch2 == ';' && ch3 == ')')
        {
            return is;
        }
    }
    is.setstate(std::ios::failbit);
    return is;
}

std::istream& operator>>(std::istream& is, Polygon& poly)
{
    int count = 0;
    if (is >> count)
    {
        if (count < 3)
        {
            is.setstate(std::ios::failbit);
            return is;
        }
        std::vector<Point> temp;
        std::copy_n(std::istream_iterator<Point>(is), count, std::back_inserter(temp));
        if (temp.size() == static_cast<size_t>(count))
        {
            poly.points = std::move(temp);
            return is;
        }
    }
    is.setstate(std::ios::failbit);
    return is;
}

struct AreaAccumulator
{
    const std::vector<Point>& points;
    AreaAccumulator(const std::vector<Point>& pts) : points(pts) {}
    double operator()(double sum, int i) const
    {
        int next = (i + 1) % points.size();
        return sum + (points[i].x * points[next].y - points[next].x * points[i].y);
    }
};

double GetPolygonArea(const Polygon& poly)
{
    if (poly.points.empty()) return 0.0;
    std::vector<int> indices(poly.points.size());
    std::iota(indices.begin(), indices.end(), 0);
    double doubleArea = std::accumulate(indices.begin(), indices.end(), 0.0, AreaAccumulator(poly.points));
    return 0.5 * std::abs(doubleArea);
}

struct RightAngleChecker
{
    const Polygon& poly;
    size_t n;
    RightAngleChecker(const Polygon& p) : poly(p), n(p.points.size()) {}
    bool operator()(int i) const
    {
        int prev = (i + n - 1) % n;
        int next = (i + 1) % n;
        long long dx1 = poly.points[prev].x - poly.points[i].x;
        long long dy1 = poly.points[prev].y - poly.points[i].y;
        long long dx2 = poly.points[next].x - poly.points[i].x;
        long long dy2 = poly.points[next].y - poly.points[i].y;
        return (dx1 * dx2 + dy1 * dy2) == 0;
    }
};

bool HasRightAngle(const Polygon& poly)
{
    if (poly.points.size() < 3)
    {
        return false;
    }
    std::vector<int> indices(poly.points.size());
    std::iota(indices.begin(), indices.end(), 0);
    return std::any_of(indices.begin(), indices.end(), RightAngleChecker(poly));
}

bool CompareArea(const Polygon& a, const Polygon& b)
{
    return GetPolygonArea(a) < GetPolygonArea(b);
}

bool CompareVertices(const Polygon& a, const Polygon& b)
{
    return a.points.size() < b.points.size();
}

bool IsEvenPoly(const Polygon& p)
{
    return p.points.size() % 2 == 0;
}

bool IsOddPoly(const Polygon& p)
{
    return p.points.size() % 2 != 0;
}

struct HasNVertices
{
    size_t n;
    HasNVertices(size_t n) : n(n) {}
    bool operator()(const Polygon& p) const
    {
        return p.points.size() == n;
    }
};

struct AddAreaEven
{
    double operator()(double acc, const Polygon& p) const
    {
        return acc + (IsEvenPoly(p) ? GetPolygonArea(p) : 0.0);
    }
};

struct AddAreaOdd
{
    double operator()(double acc, const Polygon& p) const
    {
        return acc + (IsOddPoly(p) ? GetPolygonArea(p) : 0.0);
    }
};

struct AddAreaTotal
{
    double operator()(double acc, const Polygon& p) const
    {
        return acc + GetPolygonArea(p);
    }
};

struct AddAreaN
{
    size_t n;
    AddAreaN(size_t n) : n(n) {}
    double operator()(double acc, const Polygon& p) const
    {
        return acc + (p.points.size() == n ? GetPolygonArea(p) : 0.0);
    }
};

struct BoundingBox
{
    int min_x = std::numeric_limits<int>::max();
    int max_x = std::numeric_limits<int>::min();
    int min_y = std::numeric_limits<int>::max();
    int max_y = std::numeric_limits<int>::min();
};

struct PointBoxExpander
{
    BoundingBox operator()(BoundingBox box, const Point& p) const
    {
        box.min_x = std::min(box.min_x, p.x);
        box.max_x = std::max(box.max_x, p.x);
        box.min_y = std::min(box.min_y, p.y);
        box.max_y = std::max(box.max_y, p.y);
        return box;
    }
};

BoundingBox GetPolyBox(const Polygon& poly)
{
    return std::accumulate(poly.points.begin(), poly.points.end(), BoundingBox{}, PointBoxExpander());
}

struct PolyBoxExpander
{
    BoundingBox operator()(BoundingBox box, const Polygon& p) const
    {
        BoundingBox pb = GetPolyBox(p);
        box.min_x = std::min(box.min_x, pb.min_x);
        box.max_x = std::max(box.max_x, pb.max_x);
        box.min_y = std::min(box.min_y, pb.min_y);
        box.max_y = std::max(box.max_y, pb.max_y);
        return box;
    }
};

struct IsPointInBox
{
    BoundingBox box;
    IsPointInBox(const BoundingBox& b) : box(b) {}
    bool operator()(const Point& p) const
    {
        return p.x >= box.min_x && p.x <= box.max_x &&
               p.y >= box.min_y && p.y <= box.max_y;
    }
};

void CmdArea(const std::vector<Polygon>& polygons, const std::string& arg)
{
    if (arg == "EVEN")
    {
        std::cout << std::accumulate(polygons.begin(), polygons.end(), 0.0, AddAreaEven()) << "\n";
    }
    else if (arg == "ODD")
    {
        std::cout << std::accumulate(polygons.begin(), polygons.end(), 0.0, AddAreaOdd()) << "\n";
    }
    else if (arg == "MEAN")
    {
        if (polygons.empty())
        {
            throw std::invalid_argument("");
        }
        double sum = std::accumulate(polygons.begin(), polygons.end(), 0.0, AddAreaTotal());
        std::cout << sum / polygons.size() << "\n";
    }
    else
    {
        int v = std::stoi(arg);
        if (v < 3)
        {
            throw std::invalid_argument("");
        }
        std::cout << std::accumulate(polygons.begin(), polygons.end(), 0.0, AddAreaN(v)) << "\n";
    }
}

void CmdMax(const std::vector<Polygon>& polygons, const std::string& arg)
{
    if (polygons.empty())
    {
        throw std::invalid_argument("");
    }
    if (arg == "AREA")
    {
        auto it = std::max_element(polygons.begin(), polygons.end(), CompareArea);
        std::cout << GetPolygonArea(*it) << "\n";
    }
    else if (arg == "VERTEXES")
    {
        auto it = std::max_element(polygons.begin(), polygons.end(), CompareVertices);
        std::cout << it->points.size() << "\n";
    }
    else
    {
        throw std::invalid_argument("");
    }
}

void CmdMin(const std::vector<Polygon>& polygons, const std::string& arg)
{
    if (polygons.empty())
    {
        throw std::invalid_argument("");
    }
    if (arg == "AREA")
    {
        auto it = std::min_element(polygons.begin(), polygons.end(), CompareArea);
        std::cout << GetPolygonArea(*it) << "\n";
    }
    else if (arg == "VERTEXES")
    {
        auto it = std::min_element(polygons.begin(), polygons.end(), CompareVertices);
        std::cout << it->points.size() << "\n";
    }
    else
    {
        throw std::invalid_argument("");
    }
}

void CmdCount(const std::vector<Polygon>& polygons, const std::string& arg)
{
    if (arg == "EVEN")
    {
        std::cout << std::count_if(polygons.begin(), polygons.end(), IsEvenPoly) << "\n";
    }
    else if (arg == "ODD")
    {
        std::cout << std::count_if(polygons.begin(), polygons.end(), IsOddPoly) << "\n";
    }
    else
    {
        int v = std::stoi(arg);
        if (v < 3)
        {
            throw std::invalid_argument("");
        }
        std::cout << std::count_if(polygons.begin(), polygons.end(), HasNVertices(v)) << "\n";
    }
}

void CmdInframe(const std::vector<Polygon>& polygons)
{
    std::string line;
    if (!std::getline(std::cin >> std::ws, line))
    {
        throw std::invalid_argument("");
    }

    std::stringstream ss(line);
    Polygon target;
    if (!(ss >> target))
    {
        throw std::invalid_argument("");
    }

    std::string extra;
    if (ss >> extra)
    {
        throw std::invalid_argument("");
    }

    if (polygons.empty())
    {
        std::cout << "<FALSE>\n";
        return;
    }

    BoundingBox globalBox = std::accumulate(polygons.begin(), polygons.end(), BoundingBox{}, PolyBoxExpander());
    bool inFrame = std::all_of(target.points.begin(), target.points.end(), IsPointInBox(globalBox));

    std::cout << (inFrame ? "<TRUE>\n" : "<FALSE>\n");
}

void CmdRightShapes(const std::vector<Polygon>& polygons)
{
    std::cout << std::count_if(polygons.begin(), polygons.end(), HasRightAngle) << "\n";
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Error: No filename provided.\nUsage: ./lab filename\n";
        return 1;
    }

    std::vector<Polygon> polygons;
    std::ifstream file(argv[1]);
    if (!file)
    {
        std::cerr << "Error: Could not open file " << argv[1] << "\n";
        return 1;
    }

    while (file)
    {
        Polygon p;
        if (file >> p)
        {
            polygons.push_back(std::move(p));
        }
        else
        {
            if (file.eof()) break;
            file.clear();
            file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
    file.close();

    std::cout << std::fixed << std::setprecision(1);

    std::string cmd;
    while (std::cin >> cmd)
    {
        try
        {
            if (cmd == "AREA")
            {
                std::string arg; std::cin >> arg;
                CmdArea(polygons, arg);
            }
            else if (cmd == "MAX")
            {
                std::string arg; std::cin >> arg;
                CmdMax(polygons, arg);
            }
            else if (cmd == "MIN")
            {
                std::string arg; std::cin >> arg;
                CmdMin(polygons, arg);
            }
            else if (cmd == "COUNT")
            {
                std::string arg; std::cin >> arg;
                CmdCount(polygons, arg);
            }
            else if (cmd == "INFRAME")
            {
                CmdInframe(polygons);
            }
            else if (cmd == "RIGHTSHAPES")
            {
                CmdRightShapes(polygons);
            }
            else
            {
                throw std::invalid_argument("");
            }
        }
        catch (...)
        {
            std::cout << "<INVALID COMMAND>\n";
            std::cin.clear();
            std::string discard;
            std::getline(std::cin, discard);
        }
    }

    return 0;
}
