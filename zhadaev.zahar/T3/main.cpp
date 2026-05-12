#include <iostream>
#include <string>
#include <vector>
#include <iterator>
#include <algorithm>
#include <numeric>
#include <functional>
#include <fstream>
#include <limits>
#include <iomanip>

namespace T3
{
    struct Point { int x, y; };
    struct Polygon { std::vector<Point> polygon; };
    struct Symbol { char exp; Symbol(char c) : exp(c) {} };
    std::istream& operator>>(std::istream& in, Symbol&& sym)
    {
        char c; in >> c;
        if (c != sym.exp) { in.setstate(std::ios::failbit); }
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
        int a = 0; in >> a;
        if (a < 3 || !(in))
        {
            in.setstate(std::ios::failbit);
            return in;
        }
        poly.polygon.resize(a);
        std::copy_n(std::istream_iterator<Point>(in), a, poly.polygon.begin());
        if (!(in)) { in.setstate(std::ios::failbit); }
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
        std::string mode;
        size_t ver;
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
}

using namespace T3;

int main(int argc, char* argv[])
{
    if (argc != 2) { return 1; }
    std::ifstream input(argv[1]);
    if (!(input)) { return 1; }

    std::vector<Polygon> shapes;
    Polygon figure;
    while (input >> figure)
    {
        shapes.push_back(figure);
    }
    input.clear();

    std::string command;
    std::string sub_command;
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
                if (shapes.size() == 0)
                {
                    std::cout << std::accumulate(shapes.begin(), shapes.end(), 0.0, AreaMods("MEAN")) << '\n';
                }
                else
                {
                    std::cout << std::accumulate(shapes.begin(), shapes.end(), 0.0, AreaMods("MEAN")) / shapes.size() << '\n';
                }
            }
            else if (std::isdigit(sub_command[0]))
            {
                size_t n = std::stoul(sub_command);
                if (n < 3)
                {
                    std::cout << "<INVALID COMMAND>\n";
                }
                else
                {
                    std::cout << std::accumulate(shapes.begin(), shapes.end(), 0.0, AreaMods("NUM", n)) << '\n';
                }
            }
        }
        else if (command == "MAX")
        {
            std::cin >> sub_command;

        }
        else if (command == "MIN")
        {
            std::cin >> sub_command;

        }
        else if (command == "COUNT")
        {
            std::cin >> sub_command;

        }
        else
        {
            std::cout << "<INVALID COMMAND>\n";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
    return 0;
}
