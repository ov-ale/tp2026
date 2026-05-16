#include <algorithm>
#include <functional>
#include <numeric>
#include <iostream>
#include <fstream>
#include <cctype>
#include <sstream>
#include <iomanip>
#include <limits>
#include <vector>

// var 13

struct Point
{
    int x, y;
};

struct Polygon
{
    std::vector< Point > points;
};


struct PointCrossProduct
{
    double operator()(const Point& a, const Point& b) const
    {
        return (a.x * b.y) - (b.x * a.y);
    }
};

struct SamePointCheck
{
    int dx, dy;
    SamePointCheck(int dx, int dy) : dx(dx), dy(dy) {}

    bool operator()(const Point& pa, const Point& pb) const
    {
        return (pa.x - pb.x == dx) && (pa.y - pb.y == dy);
    }
};

struct HasVertices
{
    size_t n;
    HasVertices(size_t n) : n(n) {}

    bool operator()(const Polygon& p) const
    {
        return p.points.size() == n;
    }
};

struct AddAreaIfN
{
    size_t n;
    AddAreaIfN(size_t n) : n(n) {}

    double operator()(double sum, const Polygon& p) const;
};

struct SameMatcher
{
    Polygon target;
    SameMatcher(const Polygon& target) : target(target) {}

    bool operator()(const Polygon& p) const;
};




std::istream& operator>>(std::istream& is, Point& p)
{
    char c1 = '\0', c2 = '\0', c3 = '\0';
    int x = 0;
    int y = 0;

    if(is >> c1 >> x >> c2 >> y >> c3)
    {
        if(c1 == '(' && c2 == ';' && c3 == ')')
        {
            p.x = x;
            p.y = y;
        }
        else
        {
            is.setstate(std::ios::failbit);
        }
    }
    else
    {
        is.setstate(std::ios::failbit);
    }

    return is;
}

void readPoints(std::istream& is, std::vector<Point>& points, size_t n)
{
    if (n == 0) return;

    Point p;
    if (is >> p)
    {
        points.push_back(p);
        readPoints(is, points, n - 1);
    }
    else
    {
        is.setstate(std::ios::failbit);
    }
}

std::istream& operator>>(std::istream& is, Polygon& poly)
{
    size_t n = 0;

    if(!(is >> n) || n < 3)
    {
        is.setstate(std::ios::failbit);
        return is;
    }

    std::vector<Point> temp;

    readPoints(is, temp, n);

    if (temp.size() == n)
    {
        poly.points = temp;
    }
    else
    {
        is.setstate(std::ios::failbit);
    }

    return is;
}


double PolygonArea(const Polygon& poly)
{
    if (poly.points.size() < 3) return 0.0;

    double area = std::inner_product(
        poly.points.begin(), poly.points.end() - 1,
        poly.points.begin() + 1,
        0.0,
        std::plus<double>(),
        PointCrossProduct()
    );

    area += PointCrossProduct()(poly.points.back(), poly.points.front());
    return std::abs(area) / 2.0;
}


double dotProduct(const Point& a, const Point& b, const Point& c)
{
    return (b.x - a.x) * (c.x - b.x) + (b.y - a.y) * (c.y - b.y);
}

bool isRect(const Polygon& poly)
{
    if (poly.points.size() != 4) return false;

    return dotProduct(poly.points[3], poly.points[0], poly.points[1]) == 0 &&
        dotProduct(poly.points[0], poly.points[1], poly.points[2]) == 0 &&
        dotProduct(poly.points[1], poly.points[2], poly.points[3]) == 0 &&
        dotProduct(poly.points[2], poly.points[3], poly.points[0]) == 0;
}

bool isSame(const Polygon& a, const Polygon& b)
{
    if (a.points.size() != b.points.size() || a.points.empty()) return false;

    int dx = a.points[0].x - b.points[0].x;
    int dy = a.points[0].y - b.points[0].y;

    return std::equal(a.points.begin(), a.points.end(), b.points.begin(), SamePointCheck(dx, dy));
}

double AddAreaIfN::operator()(double sum, const Polygon& p) const
{
    return p.points.size() == n ? sum + PolygonArea(p) : sum;
}

bool SameMatcher::operator()(const Polygon& p) const
{
    return isSame(p, target);
}



bool isEven(const Polygon& p) { return p.points.size() % 2 == 0; }
bool isOdd(const Polygon& p) { return p.points.size() % 2 != 0; }
double addAreaEven(double sum, const Polygon& p) { return isEven(p) ? sum + PolygonArea(p) : sum; }
double addAreaOdd(double sum, const Polygon& p) { return isOdd(p) ? sum + PolygonArea(p) : sum; }
double addAreaAll(double sum, const Polygon& p) { return sum + PolygonArea(p); }
bool compareArea(const Polygon& a, const Polygon& b) { return PolygonArea(a) < PolygonArea(b); }
bool compareVertexes(const Polygon& a, const Polygon& b) { return a.points.size() < b.points.size(); }




int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        std::cerr << "ERROR: no filename provided";
        return 1;
    }

    std::ifstream file(argv[1]);
    if(!file.is_open())
    {
        std::cerr << "ERRROR: cannot open file";
        return 1;
    }

    std::vector<Polygon> polygons;
    std::string line;

    while(std::getline(file, line))
    {
        std::istringstream iss(line);
        Polygon p;

        if(iss >> p)
        {
            std::string extra;

            if(!(iss >> extra))
            {
                polygons.push_back(p);
            }
        }
    }

    std::string cmd;
    while(std::cin >> cmd)
    {
        if(cmd == "AREA")
        {
            std::string param;
            std::cin >> param;
            std::cout << std::fixed << std::setprecision(1);


            if(param == "EVEN")
            {
                std::cout << std::accumulate(polygons.begin(), polygons.end(), 0.0, addAreaEven) << "\n";
            }
            else if(param == "ODD")
            {
                std:: cout << std::accumulate(polygons.begin(), polygons.end(), 0.0, addAreaOdd) << "\n";
            }
            else if(param == "MEAN")
            {
                if(polygons.empty())
                {
                    std::cout << "<INVALID COMMAND>\n";
                }
                else
                {
                    double total = std::accumulate(polygons.begin(), polygons.end(), 0.0, addAreaAll);
                    std::cout << total / polygons.size() << "\n";
                }
            }
            else
            {
                try
                {
                    size_t n = std::stoul(param);
                    if(n < 3)
                    {
                        std::cout << "<INVALID COMMAND>\n";
                    }
                    else
                    {
                        std::cout << std::accumulate(polygons.begin(), polygons.end(), 0.0, AddAreaIfN(n)) << "\n";
                    }
                }
                catch(...)
                {
                    std::cout << "<INVALID COMMAND>\n";
                }

            }

        }

        else if(cmd == "MAX" || cmd == "MIN")
        {
            std::string param;
            std::cin >> param;
            if(polygons.empty())
            {
                std::cout << "<INVALID COMMAND>\n";
                continue;
            }

            if(param == "AREA")
            {
                auto it = (cmd == "MAX")
                ? std::max_element(polygons.begin(), polygons.end(), compareArea)

                : std::min_element(polygons.begin(), polygons.end(), compareArea);
                std::cout << std::fixed << std::setprecision(1) << PolygonArea(*it) << "\n";
            }
            else if(param == "VERTEXES")
            {
                auto it = (cmd == "MAX")
                ? std::max_element(polygons.begin(), polygons.end(), compareVertexes)

                : std::min_element(polygons.begin(), polygons.end(), compareVertexes);
                std::cout << it->points.size() << "\n";
            }
            else
            {
                std::cout << "<INVALID COMMAND>" << "\n";
            }
        }

        else if(cmd == "COUNT")
        {
            std::string param;
            std::cin >> param;

            if(param == "EVEN")
            {
                std::cout << std::count_if(polygons.begin(), polygons.end(), isEven) << "\n";
            }
            else if(param == "ODD")
            {
                std::cout << std::count_if(polygons.begin(), polygons.end(), isOdd) << "\n";
            }
            else
            {
                try
                {
                    size_t n = std::stoul(param);
                    if(n < 3)
                    {
                        std::cout << "<INVALID COMMAND>\n";
                        continue;
                    }
                    std::cout << std::count_if(polygons.begin(), polygons.end(), HasVertices(n)) << "\n";
                }
                catch(...)
                {
                    std::cout << "<INVALID COMMAND>\n";
                }

            }

        }
        else if(cmd == "RECTS")
        {
            std::cout << std::count_if(polygons.begin(), polygons.end(), isRect) << "\n";
        }
        else if(cmd == "SAME")
        {
            std::string sameline;
            std::getline(std::cin, sameline);
            std::istringstream iss(sameline);

            Polygon target;
            if (iss >> target)
            {
                std::string extra;
                if (!(iss >> extra))
                {
                    std::cout << std::count_if(polygons.begin(), polygons.end(), SameMatcher(target)) << "\n";
                }
                else std::cout << "<INVALID COMMAND>\n";
            }
            else std::cout << "<INVALID COMMAND>\n";
        }
        else
        {
            std::cout << "<INVALID COMMAND>\n";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    return 0;
}
