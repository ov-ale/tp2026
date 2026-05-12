#include <iostream>
#include <vector>
#include <iterator>

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
        int a; in >> a;
        if (a < 3)
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
}

using namespace T3;

int main()
{
    std::vector<Polygon> shapes;
    Polygon figure;
    while (std::cin >> figure)
    {
        shapes.push_back(figure);
    }
    for (int i = 0; i < shapes.size(); i++)
    {
        std::cout << shapes[i] << '\n';
    }
    return 0;
}
// 3 (1;1) (2;2) (3;3)
// 4 (1;1) (2;2) (3;3) (4;4)
// 5 (1;1) (2;2) (3;3) (4;4) (5;5)
// 6 (1;1) (2;2) (3;3) (4;4) (5;5) (6;6)
