#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <numeric>
#include <iomanip>
#include <iterator>
#include <algorithm>
#include <cctype>
#include <cmath>

namespace T3
{
    struct Point
    {
        int x, y;
    };

    struct Polygon
    {
        std::vector<Point> poly;
    };

    struct Symbol
    {
        char exp;
        Symbol(char c) : exp(c) {}
    };

    std::istream& operator>>(std::istream& in, Symbol&& symbol)
    {
        char c;
        in >> c;
        if (in && c != symbol.exp)
        {
            in.setstate(std::ios::failbit);
        }
        return in;
    }

    std::istream& operator>>(std::istream& in, Point& point)
    {
        in >> Symbol('(') >> point.x >> Symbol(';') >> point.y >> Symbol(')');
        return in;
    }

    std::istream& operator>>(std::istream& in, Polygon& pol)
    {
        std::istream::sentry sentry(in);
        if (!sentry) return in;

        size_t vertexCount;
        if (!(in >> vertexCount) || vertexCount < 3)
        {
            in.setstate(std::ios::failbit);
            return in;
        }

        std::vector<Point> tempPoints;
        for (size_t i = 0; i < vertexCount; ++i)
        {
            Point p;
            if (in >> p) tempPoints.push_back(p);
            else
            {
                in.setstate(std::ios::failbit);
                return in;
            }
        }

        pol.poly = std::move(tempPoints);
        return in;
    }


    double getArea(const Polygon& pol)
    {
        const auto& pts = pol.poly;
        double area = 0.0;
        for (size_t i = 0; i < pts.size(); ++i)
        {
            const Point& p1 = pts[i];
            const Point& p2 = pts[(i + 1) % pts.size()];
            area += static_cast<double>(p1.x) * p2.y - static_cast<double>(p2.x) * p1.y;
        }
        return std::abs(area) / 2.0;
    }

    int getOrientation(const Point& p, const Point& q, const Point& r)
    {
        long long val = 1LL * (q.y - p.y) * (r.x - q.x) - 1LL * (q.x - p.x) * (r.y - q.y);
        if (val == 0) return 0;
        return (val > 0) ? 1 : 2;
    }

    bool doIntersect(const Point& p1, const Point& q1, const Point& p2, const Point& q2)
    {
        int o1 = getOrientation(p1, q1, p2);
        int o2 = getOrientation(p1, q1, q2);
        int o3 = getOrientation(p2, q2, p1);
        int o4 = getOrientation(p2, q2, q1);
        return (o1 != o2 && o3 != o4);
    }

    bool arePolygonsEqual(const Polygon& a, const Polygon& b)
    {
        if (a.poly.size() != b.poly.size()) return false;
        return std::equal(a.poly.begin(), a.poly.end(), b.poly.begin(),
            [](const Point& p1, const Point& p2) {
                return p1.x == p2.x && p1.y == p2.y;
            });
    }


    struct AreaSummator
    {
        std::string mode;
        size_t num;
        AreaSummator(std::string m, size_t n = 0) : mode(m), num(n) {}
        double operator()(double current_sum, const Polygon& pol) const
        {
            bool cond = false;
            if (mode == "EVEN") cond = (pol.poly.size() % 2 == 0);
            else if (mode == "ODD") cond = (pol.poly.size() % 2 != 0);
            else if (mode == "NUM") cond = (pol.poly.size() == num);
            else if (mode == "MEAN") cond = true;
            return cond ? (current_sum + getArea(pol)) : current_sum;
        }
    };

    struct CountSummator
    {
        std::string mode;
        size_t num;
        CountSummator(std::string m, size_t n = 0) : mode(m), num(n) {}
        size_t operator()(size_t count, const Polygon& pol) const
        {
            bool cond = false;
            if (mode == "EVEN") cond = (pol.poly.size() % 2 == 0);
            else if (mode == "ODD") cond = (pol.poly.size() % 2 != 0);
            else if (mode == "NUM") cond = (pol.poly.size() == num);
            return cond ? (count + 1) : count;
        }
    };

    struct IntersectionsPredicate
    {
        const Polygon& target;
        IntersectionsPredicate(const Polygon& t) : target(t) {}
        bool operator()(const Polygon& current) const
        {
            for (size_t i = 0; i < current.poly.size(); ++i) {
                for (size_t j = 0; j < target.poly.size(); ++j) {
                    if (doIntersect(current.poly[i], current.poly[(i + 1) % current.poly.size()],
                        target.poly[j], target.poly[(j + 1) % target.poly.size()]))
                        return true;
                }
            }
            return false;
        }
    };

    struct EchoComparator
    {
        const Polygon& target;
        EchoComparator(const Polygon& t) : target(t) {}
        bool operator()(const Polygon& a, const Polygon& b) const {
            return arePolygonsEqual(a, target) && arePolygonsEqual(b, target);
        }
    };

    struct AreaComparator {
        bool operator()(const Polygon& a, const Polygon& b) const { return getArea(a) < getArea(b); }
    };

    struct VertexComparator {
        bool operator()(const Polygon& a, const Polygon& b) const { return a.poly.size() < b.poly.size(); }
    };
}

int main()
{
    std::vector<T3::Polygon> shapes;

    while (std::cin >> std::ws)
    {
        if (!std::isdigit(std::cin.peek())) break;
        T3::Polygon temp;
        if (std::cin >> temp) shapes.push_back(std::move(temp));
        else
        {
            std::cin.clear();
            std::string skip;
            std::getline(std::cin, skip);
        }
    }

    std::string command;
    while (std::cin >> command)
    {
        try
        {
            if (command == "AREA")
            {
                std::string sub; std::cin >> sub;
                std::cout << std::fixed << std::setprecision(1);
                if (sub == "EVEN" || sub == "ODD")
                    std::cout << std::accumulate(shapes.begin(), shapes.end(), 0.0, T3::AreaSummator(sub)) << "\n";
                else if (sub == "MEAN")
                {
                    if (shapes.empty()) throw std::invalid_argument("");
                    std::cout << std::accumulate(shapes.begin(), shapes.end(), 0.0, T3::AreaSummator("MEAN")) / shapes.size() << "\n";
                }
                else
                {
                    size_t n = std::stoul(sub);
                    if (n < 3) throw std::invalid_argument("");
                    std::cout << std::accumulate(shapes.begin(), shapes.end(), 0.0, T3::AreaSummator("NUM", n)) << "\n";
                }
            }
            else if (command == "MAX")
            {
                std::string sub; std::cin >> sub;
                if (shapes.empty()) throw std::invalid_argument("");
                if (sub == "AREA") std::cout << std::fixed << std::setprecision(1)
                    << T3::getArea(*std::max_element(shapes.begin(), shapes.end(), T3::AreaComparator())) << "\n";
                else if (sub == "VERTICES") std::cout << std::max_element(shapes.begin(), shapes.end(), T3::VertexComparator())->poly.size() << "\n";
                else throw std::invalid_argument("");
            }
            else if (command == "MIN")
            {
                std::string sub; std::cin >> sub;
                if (shapes.empty()) throw std::invalid_argument("");
                if (sub == "AREA") std::cout << std::fixed << std::setprecision(1)
                    << T3::getArea(*std::min_element(shapes.begin(), shapes.end(), T3::AreaComparator())) << "\n";
                else if (sub == "VERTICES") std::cout << std::min_element(shapes.begin(), shapes.end(), T3::VertexComparator())->poly.size() << "\n";
                else throw std::invalid_argument("");
            }
            else if (command == "COUNT")
            {
                std::string sub; std::cin >> sub;
                if (sub == "EVEN" || sub == "ODD")
                    std::cout << std::accumulate(shapes.begin(), shapes.end(), 0, T3::CountSummator(sub)) << "\n";
                else
                {
                    size_t n = std::stoul(sub);
                    if (n < 3) throw std::invalid_argument("");
                    std::cout << std::accumulate(shapes.begin(), shapes.end(), 0, T3::CountSummator("NUM", n)) << "\n";
                }
            }
            else if (command == "INTERSECTIONS")
            {
                T3::Polygon target;
                if (!(std::cin >> target)) throw std::invalid_argument("");
                std::cout << std::count_if(shapes.begin(), shapes.end(), T3::IntersectionsPredicate(target)) << "\n";
            }
            else if (command == "RMECHO")
            {
                T3::Polygon target;
                if (!(std::cin >> target)) throw std::invalid_argument("");
                size_t oldSize = shapes.size();
                auto it = std::unique(shapes.begin(), shapes.end(), T3::EchoComparator(target));
                shapes.erase(it, shapes.end());
                std::cout << oldSize - shapes.size() << "\n";
            }
            else throw std::invalid_argument("");
        }
        catch (...)
        {
            std::cout << "<INVALID COMMAND>\n";
            std::cin.clear();
            std::string ignore;
            std::getline(std::cin, ignore);
        }
    }
    return 0;
}
