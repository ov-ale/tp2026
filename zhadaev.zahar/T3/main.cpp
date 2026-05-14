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
            if (shapes.empty()) { std::cout << "<INVALID COMMAND>\n"; }
            else
                if (sub_command == "EVEN")
                {
                    if (sub_command == "EVEN")
                    {
                        std::cout << std::count_if(shapes.begin(), shapes.end(), Count("EVEN")) << '\n';
                    }
                    else if (sub_command == "ODD")
                    {
                        std::cout << std::count_if(shapes.begin(), shapes.end(), Count("ODD")) << '\n';
                    }
                    else if (std::isdigit(sub_command[0]))
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
                        size_t n = std::stoul(sub_command);
                        if (n < 3) { std::cout << "<INVALID COMMAND>\n"; }
                        else
                        {
                            std::cout << std::count_if(shapes.begin(), shapes.end(), Count("NUM", n)) << '\n';
                        }
                        std::cout << std::count_if(shapes.begin(), shapes.end(), Count("NUM", n)) << '\n';
                    }
                }
        }
        else
        {
            std::cout << "<INVALID COMMAND>\n";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
    return 0;
}
