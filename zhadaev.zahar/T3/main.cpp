#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <numeric>
#include <iomanip>
#include <algorithm>

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
        if (c != symbol.exp)
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
        if (!sentry)
        {
            return in;
        }
        int a;
        in >> a;
        if (a < 3)
        {
            in.setstate(std::ios::failbit);
            return in;
        }
        pol.poly.resize(a);
        std::copy_n(std::istream_iterator<Point>(in), a, pol.poly.begin());
        if (!in) {
            in.setstate(std::ios::failbit);
        }
        return in;
    }

    struct Area
    {
        double operator()(double current_sum, const Point& p1) const
        {
            const Point* p2 = &p1 + 1;
            double res = static_cast<double>(p1.x) * p2->y - static_cast<double>(p2->x) * p1.y;
            return current_sum + res;
        }
    };

    double getArea(const Polygon& pol)
    {
        double area = std::accumulate(pol.poly.begin(), pol.poly.end() - 1, 0.0, Area());
        const Point& last = pol.poly.back();
        const Point& first = pol.poly.front();
        area += (static_cast<double>(last.x) * first.y - static_cast<double>(first.x) * last.y);
        return std::abs(area) / 2.0;
    }

    struct AreaSummator
    {
        std::string mode;
        size_t num;
        AreaSummator(std::string m, size_t n = 0) : mode(m), num(n) {}
        double operator()(double current_sum, const Polygon& pol) const
        {
            bool condition = false;
            if (mode == "EVEN")
            {
                condition = (pol.poly.size() % 2 == 0);
            }
            else if (mode == "ODD")
            {
                condition = (pol.poly.size() % 2 != 0);
            }
            else if (mode == "NUM")
            {
                condition = (pol.poly.size() == num);
            }
            else if (mode == "MEAN")
            {
                condition = true;
            }
            if (condition) {
                return current_sum + getArea(pol);
            }
            return current_sum;
        }
    };

    struct AreaComparator
    {
        bool operator()(const Polygon& a, const Polygon& b) const
        {
            return getArea(a) < getArea(b);
        }
    };

    struct VertexComparator
    {
        bool operator()(const Polygon& a, const Polygon& b) const
        {
            return a.poly.size() < b.poly.size();
        }
    };

    struct CountSummator
    {
        std::string mode;
        size_t num;
        CountSummator(std::string m, size_t n = 0) : mode(m), num(n) {}
        size_t operator()(size_t current_count, const Polygon& pol) const {
            bool condition = false;
            size_t vertices = pol.poly.size();
            if (mode == "EVEN")
            {
                condition = (vertices % 2 == 0);
            }
            else if (mode == "ODD")
            {
                condition = (vertices % 2 != 0);
            }
            else if (mode == "NUM")
            {
                condition = (vertices == num);
            }
            return condition ? (current_count + 1) : current_count;
        }
    };

    int getOrientation(const Point& p, const Point& q, const Point& r)
    {
        long long val = 1LL * (q.y - p.y) * (r.x - q.x) - 1LL * (q.x - p.x) * (r.y - q.y);
        if (val == 0) return 0;
        return (val > 0) ? 1 : 2;
    }

    bool doIntersect(const Point& p1, const Point& q1, const Point& p2, const Point& q2) {
        int o1 = getOrientation(p1, q1, p2);
        int o2 = getOrientation(p1, q1, q2);
        int o3 = getOrientation(p2, q2, p1);
        int o4 = getOrientation(p2, q2, q1);

        if (o1 != o2 && o3 != o4) return true;
        return false;
    }

    struct IntersectionsPredicate
    {
        const Polygon& target;
        IntersectionsPredicate(const Polygon& t) : target(t) {}
        bool operator()(const Polygon& current) const
        {
            return std::any_of(current.poly.begin(), current.poly.end(), [&](const Point& p1) {
                const Point& q1 = (&p1 == &current.poly.back()) ? current.poly.front() : *(&p1 + 1);
                
                return std::any_of(target.poly.begin(), target.poly.end(), [&](const Point& p2) {
                    const Point& q2 = (&p2 == &target.poly.back()) ? target.poly.front() : *(&p2 + 1);
                    return doIntersect(p1, q1, p2, q2);
                    });
                });
        }
    };

    struct PointComparator
    {
        bool operator()(const Point& a, const Point& b) const
        {
            return (a.x == b.x && a.y == b.y);
        }
    };
    bool arePolygonsEqual(const Polygon& a, const Polygon& b)
    {
        if (a.poly.size() != b.poly.size())
        {
            return false;
        }
        return std::equal(a.poly.begin(), a.poly.end(), b.poly.begin(), PointComparator());
    }
    struct EchoComparator
    {
        const Polygon& target;

        EchoComparator(const Polygon& t) : target(t) {}

        bool operator()(const Polygon& a, const Polygon& b) const
        {
            if (arePolygonsEqual(a, target) && arePolygonsEqual(b, target))
            {
                return true;
            }
            return false;
        }
    };
}

using namespace T3;

int main()
{
    std::vector<Polygon> shapes;
    std::string line;
    while (std::getline(std::cin, line))
    {
        std::stringstream ss(line);
        Polygon temp;
        if (ss >> temp)
        {
            std::string extra;
            if (!(ss >> extra))
            {
                shapes.push_back(std::move(temp));
            }
        }
    }
    std::cin.clear();

    std::string command;
    while (std::cin >> command)
    {
        if (command == "AREA")
        {
            std::string sub_command;
            std::cin >> sub_command;
            std::cout << std::fixed << std::setprecision(1);

            if (sub_command == "EVEN")
            {
                double res = std::accumulate(shapes.begin(), shapes.end(), 0.0, AreaSummator("EVEN"));
                std::cout << res << "\n";
            }
            else if (sub_command == "ODD")
            {
                double res = std::accumulate(shapes.begin(), shapes.end(), 0.0, AreaSummator("ODD"));
                std::cout << res << "\n";
            }
            else if (sub_command == "MEAN")
            {
                if (shapes.empty())
                {
                    std::cout << "<INVALID COMMAND>\n";
                }
                else
                {
                    double total = std::accumulate(shapes.begin(), shapes.end(), 0.0, AreaSummator("MEAN"));
                    std::cout << total / shapes.size() << "\n";
                }
            }
            else
            {
                try
                {
                    size_t num = std::stoul(sub_command);
                    if (num < 3) {
                        std::cout << "<INVALID COMMAND>\n";
                    }
                    else
                    {
                        double res = std::accumulate(shapes.begin(), shapes.end(), 0.0, AreaSummator("NUM", num));
                        std::cout << res << "\n";
                    }
                }
                catch (...)
                {
                    std::cout << "<INVALID COMMAND>\n";
                }
            }
        }
        if (command == "MAX")
        {
            std::string sub_command;
            std::cin >> sub_command;

            if (shapes.empty())
            {
                std::cout << "<INVALID COMMAND>\n";
            }
            else if (sub_command == "AREA")
            {
                auto it = std::max_element(shapes.begin(), shapes.end(), AreaComparator());
                std::cout << std::fixed << std::setprecision(1);
                std::cout << getArea(*it) << "\n";
            }
            else if (sub_command == "VERTICES")
            {
                auto it = std::max_element(shapes.begin(), shapes.end(), VertexComparator());
                std::cout << it->poly.size() << "\n";
            }
            else {
                std::cout << "<INVALID COMMAND>\n";
            }
        }
        if (command == "MIN")
        {
            std::string sub_command;
            std::cin >> sub_command;

            if (shapes.empty())
            {
                std::cout << "<INVALID COMMAND>\n";
            }
            else if (sub_command == "AREA")
            {
                auto it = std::min_element(shapes.begin(), shapes.end(), AreaComparator());
                std::cout << std::fixed << std::setprecision(1) << getArea(*it) << "\n";
            }
            else if (sub_command == "VERTICES")
            {
                auto it = std::min_element(shapes.begin(), shapes.end(), VertexComparator());
                std::cout << it->poly.size() << "\n";
            }
        }
        if (command == "COUNT")
        {
            std::string sub_command;
            std::cin >> sub_command;
            if (sub_command == "EVEN")
            {
                size_t res = std::accumulate(shapes.begin(), shapes.end(), 0, CountSummator("EVEN"));
                std::cout << res << "\n";
            }
            else if (sub_command == "ODD")
            {
                size_t res = std::accumulate(shapes.begin(), shapes.end(), 0, CountSummator("ODD"));
                std::cout << res << "\n";
            }
            else
            {
                try
                {
                    size_t num = std::stoul(sub_command);
                    size_t res = std::accumulate(shapes.begin(), shapes.end(), 0, CountSummator("NUM", num));
                    std::cout << res << "\n";
                }
                catch (...)
                {
                    std::cout << "<INVALID COMMAND>\n";
                }
            }
        }
        if (command == "INTERSECTIONS")
        {
            Polygon target;
            if (std::cin >> target)
            {
                long long count = std::count_if(shapes.begin(), shapes.end(), IntersectionsPredicate(target));
                std::cout << count << "\n";
            }
            else
            {
                std::cout << "<INVALID COMMAND>\n";
                std::cin.clear();
                std::cin.ignore(10000, '\n');
            }
        }
        if (command == "RMECHO")
        {
            Polygon target;
            if (std::cin >> target)
            {
                size_t initial_size = shapes.size();
                auto it = std::unique(shapes.begin(), shapes.end(), EchoComparator(target));
                shapes.erase(it, shapes.end());
                std::cout << initial_size - shapes.size() << "\n";
            }
        }
    }
    return 0;
}
