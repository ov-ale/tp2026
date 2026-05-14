#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <iterator>
#include <algorithm>
#include <numeric>
#include <functional>
#include <fstream>
#include <limits>
#include <iomanip>
#include <utility>

namespace T3
{
    struct Point { int x, y; };
    struct Polygon { std::vector<Point> polygon; };
    struct Symbol { char exp; Symbol(char c) : exp(c) {} };
    std::istream& operator>>(std::istream& in, Symbol&& sym)
    {
        char c = 0;
        if (in >> c && c != sym.exp) { in.setstate(std::ios::failbit); }
        return in;
    }
    std::istream& operator>>(std::istream& in, Point& point)
    {
        in >> Symbol('(') >> point.x >> Symbol(';') >> point.y >> Symbol(')');
        return in;
    }
    std::ostream& operator<<(std::ostream& out, const Point& point)
    {
        out << '(' << point.x << ';' << point.y << ") ";
        return out;
    }
    std::istream& operator>>(std::istream& in, Polygon& poly)
    {
        std::string line;
        if (!std::getline(in, line))
        {
            in.setstate(std::ios::failbit);
            return in;
        }
        std::istringstream iss(line);
        size_t a = 0;
        if (!(iss >> a) || a < 3)
        {
            in.setstate(std::ios::failbit);
            return in;
        }

        poly.polygon.clear();
        poly.polygon.resize(a);

        struct PointReader
        {
            std::istream& stream;
            bool failed = false;

            PointReader(std::istream& s) : stream(s) {}

            Point operator()()
            {
                Point p{ 0, 0 };
                if (!failed && !(stream >> p))
                    failed = true;
                return p;
            }
        };

        PointReader reader(iss);
        std::generate_n(poly.polygon.begin(), a, std::ref(reader));
        if (reader.failed || iss.fail())
        {
            poly.polygon.clear();
            in.setstate(std::ios::failbit);
            return in;
        }
        std::string trash;
        if (iss >> trash)
        {
            poly.polygon.clear();
            in.setstate(std::ios::failbit);
            return in;
        }
        return in;
    }
    std::ostream& operator<<(std::ostream& out, Polygon& poly)
    {
        out << poly.polygon.size() << " ";
        std::copy(
            poly.polygon.begin(),
            poly.polygon.end(),
            std::ostream_iterator<Point>(out)
        );
        return out;
    }
    struct EdgeArea
    {
        double operator()(const Point& p1, const Point& p2)
        {
            return (static_cast<double>(p1.x) * p2.y - static_cast<double>(p1.y) * p2.x);
        }
    };
    struct PolygonArea
    {
        double operator()(const Polygon& poly)
        {
            std::vector<Point> shifted(poly.polygon.size());
            std::copy(poly.polygon.begin() + 1, poly.polygon.end(), shifted.begin());
            shifted.back() = poly.polygon.front();
            auto sum = std::inner_product(poly.polygon.begin(), poly.polygon.end(), shifted.begin(), 0.0, std::plus<double>(), EdgeArea());
            return std::abs(sum) / 2.0;
        }
    };
    struct AreaMods
    {
        std::string mode; size_t ver;
        AreaMods(std::string str, size_t num = 0) : mode(str), ver(num) {}
        double operator()(double currect, const Polygon& poly)
        {
            if (mode == "EVEN")
            {
                if (poly.polygon.size() % 2 == 0)
                {
                    return currect += PolygonArea()(poly);
                }
            }
            else if (mode == "ODD")
            {
                if (poly.polygon.size() % 2 != 0)
                {
                    return currect += PolygonArea()(poly);
                }
            }
            else if (mode == "MEAN")
            {
                return currect += PolygonArea()(poly);
            }
            else if (mode == "NUM")
            {
                if (ver == poly.polygon.size())
                {
                    return currect += PolygonArea()(poly);
                }
            }
            return currect;
        }
    };
    struct Max_Min
    {
        std::string command;
        Max_Min(std::string str) : command(str) {}
        bool operator()(const Polygon& a, const Polygon& b)
        {
            if (command == "AREA")
            {
                return PolygonArea()(a) < PolygonArea()(b);
            }
            if (command == "VERTEXES")
            {
                return a.polygon.size() < b.polygon.size();
            }
            return false;
        }
    };
    struct Count
    {
        std::string command; size_t n;
        Count(std::string str, size_t ver = 0) : command(str), n(ver) {}
        bool operator()(const Polygon& poly)
        {
            if (command == "EVEN")
            {
                return poly.polygon.size() % 2 == 0;
            }
            else if (command == "ODD")
            {
                return poly.polygon.size() % 2 != 0;

            }
            else if (command == "NUM")
            {
                return poly.polygon.size() == n;
            }
            return false;
        }
    };
    bool operator==(const Polygon& a, const Polygon& b)
    {
        if (a.polygon.size() != b.polygon.size()) return false;
        return std::equal(a.polygon.begin(), a.polygon.end(), b.polygon.begin());
    }
    bool operator==(const Point& a, const Point& b)
    {
        return a.x == b.x && a.y == b.y;
    }
    long long pseudoscalar(Point a, Point b, Point c)
    {
        return (1LL * (b.x - a.x) * (c.y - a.y) - 1LL * (b.y - a.y) * (c.x - a.x));
    }
    bool pointOnSegment(Point a, Point b, Point p)
    {
        return (p.x >= std::min(a.x, b.x) && p.x <= std::max(a.x, b.x) &&
            p.y >= std::min(a.y, b.y) && p.y <= std::max(a.y, b.y));
    }
    bool checkSegmentsIntersect(Point x1, Point y1, Point x2, Point y2)
    {
        long long d1 = pseudoscalar(x1, y1, x2);
        long long d2 = pseudoscalar(x1, y1, y2);
        long long d3 = pseudoscalar(x2, y2, x1);
        long long d4 = pseudoscalar(x2, y2, y1);

        if (((d1 > 0 && d2 < 0) || (d2 > 0 && d1 < 0)) &&
            ((d3 > 0 && d4 < 0) || (d4 > 0 && d3 < 0)))
            return true;

        if (d1 == 0 && pointOnSegment(x1, y1, x2)) return true;
        if (d2 == 0 && pointOnSegment(x1, y1, y2)) return true;
        if (d3 == 0 && pointOnSegment(x2, y2, x1)) return true;
        if (d4 == 0 && pointOnSegment(x2, y2, y1)) return true;

        return false;
    }
    using Segment = std::pair<Point, Point>;

    Segment makeSegment(Point a, Point b)
    {
        return std::make_pair(a, b);
    }

    std::vector<Segment> getSegments(const Polygon& pg)
    {
        std::vector<Segment> segments;
        if (pg.polygon.empty()) return segments;

        std::transform(pg.polygon.begin(), pg.polygon.end() - 1,
            pg.polygon.begin() + 1,
            std::back_inserter(segments), makeSegment);
        segments.push_back(makeSegment(pg.polygon.back(), pg.polygon.front()));
        return segments;
    }
    bool checkSegmentsIntersectForPair(const Segment& sg, Point ra, Point rb)
    {
        return checkSegmentsIntersect(sg.first, sg.second, ra, rb);
    }

    bool segmentsIntersectWrapper(const Segment& s, const Polygon& pg)
    {
        auto segs = getSegments(pg);
        return std::any_of(segs.begin(), segs.end(),
            std::bind(checkSegmentsIntersectForPair, std::placeholders::_1, s.first, s.second));
    }
    bool isPointInside(Point a, const Polygon& pg, int max_x)
    {
        auto segments = getSegments(pg);
        Point rayEnd = { max_x + 1, a.y + 1 };
        int intersections = std::count_if(segments.begin(), segments.end(),
            std::bind(checkSegmentsIntersectForPair, std::placeholders::_1, a, rayEnd));
        return (intersections % 2 == 1);
    }
    bool pointInsideWrapper(Point p, const Polygon& poly, int max_x)
    {
        return isPointInside(p, poly, max_x);
    }
    bool comparePointX(Point a, Point b)
    {
        return a.x < b.x;
    }
    int getPolygonMaxX(const Polygon& p)
    {
        if (p.polygon.empty()) return 0;
        return std::max_element(p.polygon.begin(), p.polygon.end(), comparePointX)->x;
    }
    bool comparePolygonMaxX(const Polygon& a, const Polygon& b)
    {
        return getPolygonMaxX(a) < getPolygonMaxX(b);
    }
    bool polygonsIntersect(const Polygon& p1, const Polygon& p2, int max_x)
    {
        auto segments = getSegments(p1);
        if (std::any_of(segments.begin(), segments.end(),
            std::bind(segmentsIntersectWrapper, std::placeholders::_1, p2)))
            return true;
        if (std::any_of(p1.polygon.begin(), p1.polygon.end(),
            std::bind(pointInsideWrapper, std::placeholders::_1, p2, max_x)))
            return true;
        if (std::any_of(p2.polygon.begin(), p2.polygon.end(),
            std::bind(pointInsideWrapper, std::placeholders::_1, p1, max_x)))
            return true;

        return false;
    }
    int getGlobalMaxX(const std::vector<Polygon>& polys)
    {
        if (polys.empty()) return 0;
        return getPolygonMaxX(*std::max_element(polys.begin(), polys.end(), comparePolygonMaxX));
    }
}

using namespace T3;

int main(int argc, char* argv[])
{
    if (argc != 2) { return 1; }
    std::ifstream input(argv[1]);
    if (!(input)) { return 1; }

    std::vector<Polygon> shapes;
    Polygon figure;


    std::string line;
    while (std::getline(input, line))
    {
        std::stringstream ss(line);
        if (ss >> figure)
        {
            shapes.push_back(figure);
        }
    }
    input.clear();
    int globalMaxX = getGlobalMaxX(shapes);
    std::string command, sub_command;
    while (std::cin >> command)
    {
        if (command == "AREA")
        {
            std::cin >> sub_command;
            std::cout << std::fixed << std::setprecision(1);
            if (sub_command == "EVEN")
            {
                std::cout << std::accumulate(shapes.begin(), shapes.end(), 0.0, AreaMods("EVEN")) << '\n';
            }
            else if (sub_command == "ODD")
            {
                std::cout << std::accumulate(shapes.begin(), shapes.end(), 0.0, AreaMods("ODD")) << '\n';
            }
            else if (sub_command == "MEAN")
            {
                if (shapes.empty()) { std::cout << "<INVALID COMMAND>\n"; }
                else
                {
                    std::cout << std::accumulate(shapes.begin(), shapes.end(), 0.0, AreaMods("MEAN")) / shapes.size() << '\n';
                }
            }
            else if (std::isdigit(sub_command[0]))
            {
                size_t n = std::stoul(sub_command);
                if (n < 3) { std::cout << "<INVALID COMMAND>\n"; }
                else
                {
                    std::cout << std::accumulate(shapes.begin(), shapes.end(), 0.0, AreaMods("NUM", n)) << '\n';
                }
            }
        }
        else if (command == "MAX")
        {
            std::cin >> sub_command;
            std::cout << std::fixed << std::setprecision(1);
            if (shapes.empty()) { std::cout << "<INVALID COMMAND>\n"; }
            else
            {
                if (sub_command == "AREA")
                {
                    std::cout << PolygonArea()(*(std::max_element(shapes.begin(), shapes.end(), Max_Min("AREA")))) << '\n';
                }
                else if (sub_command == "VERTEXES")
                {
                    std::cout << std::max_element(shapes.begin(), shapes.end(), Max_Min("VERTEXES"))->polygon.size() << '\n';
                }
            }
        }
        else if (command == "MIN")
        {
            std::cin >> sub_command;
            std::cout << std::fixed << std::setprecision(1);
            if (shapes.empty()) { std::cout << "<INVALID COMMAND>\n"; }
            else
            {
                if (sub_command == "AREA")
                {
                    std::cout << PolygonArea()(*(std::min_element(shapes.begin(), shapes.end(), Max_Min("AREA")))) << '\n';
                }
                else if (sub_command == "VERTEXES")
                {
                    std::cout << std::min_element(shapes.begin(), shapes.end(), Max_Min("VERTEXES"))->polygon.size() << '\n';
                }
            }
        }
        else if (command == "COUNT")
        {
            std::cin >> sub_command;
            if (sub_command == "EVEN")
            {
                std::cout << std::count_if(shapes.begin(), shapes.end(), Count("EVEN")) << '\n';
            }
            else if (sub_command == "ODD")
            {
                std::cout << std::count_if(shapes.begin(), shapes.end(), Count("ODD")) << '\n';
            }
            else if (std::isdigit(sub_command[0]))
            {
                size_t n = std::stoul(sub_command);
                if (n < 3) { std::cout << "<INVALID COMMAND>\n"; }
                else
                {
                    std::cout << std::count_if(shapes.begin(), shapes.end(), Count("NUM", n)) << '\n';
                }
            }
        }
        else if (command == "RMECHO")
        {
            Polygon target;
            std::string restOfLine;
            std::getline(std::cin, restOfLine);

            std::istringstream iss(restOfLine);
            if (!(iss >> target))
            {
                std::cout << "<INVALID COMMAND>\n";
                continue;
            }

            size_t removed = 0;
            bool prevWasTarget = false;

            auto newEnd = std::remove_if(shapes.begin(), shapes.end(),
                [&target, &prevWasTarget, &removed](const Polygon& p) -> bool
                {
                    if (p == target && prevWasTarget)
                    {
                        ++removed;
                        return true;
                    }
                    prevWasTarget = (p == target);
                    return false;
                });

            shapes.erase(newEnd, shapes.end());
            std::cout << removed << '\n';
        }
        else if (command == "INTERSECTIONS")
        {
            Polygon target;
            std::string restOfLine;
            std::getline(std::cin, restOfLine);

            std::istringstream iss(restOfLine);
            if (!(iss >> target))
            {
                std::cout << "<INVALID COMMAND>\n";
                continue;
            }

            int currentMaxX = std::max(globalMaxX, getPolygonMaxX(target));

            auto count = std::count_if(shapes.begin(), shapes.end(),
                [&target, currentMaxX](const Polygon& p)
                {
                    return polygonsIntersect(target, p, currentMaxX);
                });

            std::cout << count << '\n';
        }
        else
        {
            std::cout << "<INVALID COMMAND>\n";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
    return 0;
}
