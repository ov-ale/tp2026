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

std::istream& operator>>(std::istream& is, Polygon& poly)
{
    size_t n = 0;

    if(!(is >> n) || n < 3)
    {
        is.setstate(std::ios::failbit);
        return is;
    }

    poly.points.clear();
    poly.points.resize(n);

    for (size_t i = 0; i < n; i++)
    {
        if (!(is >> poly.points[i]))
        {
            is.setstate(std::ios::failbit);
            return is;
        }
    }

    return is;
}

double PolygonArea(const Polygon& poly)
{
    const auto& pts = poly.points;

    double area = std::abs(
        std::accumulate(pts.begin(), pts.end(), 0.0,
            [&pts](double sum, const Point& p)
            {
                const Point& next = (&p == &pts.back()) ? pts.front() : *(&p + 1);
                return sum + (p.x * next.y - next.x * p.y);
            })
    ) * 0.5;

    return area;
}

bool isRect(const Polygon& poly)
{
    if(poly.points.size() != 4)
    {
        return false;
    }

    const auto& p = poly.points;

    auto dot = [](const Point&a, const Point& b, const Point& c)
    {
        return (b.x - a.x) * (c.x - b.x) + (b.y - a.y) * (c.y - b.y);
    };

    return dot(p[3], p[0], p[1]) == 0 && dot(p[0], p[1], p[2]) == 0 && dot(p[1], p[2], p[3]) == 0 && dot(p[2], p[3], p[0]) == 0;
}

bool isSame(const Polygon& a, const Polygon& b)
{
    if(a.points.size() != b.points.size())
    {
        return false;
    }

    int dx = a.points[0].x - b.points[0].x;
    int dy = a.points[0].y - b.points[0].y;

    return std::equal(a.points.begin(), a.points.end(), b.points.begin(),
        [dx, dy](const Point& pa, const Point& pb)
        {
            return (pa.x - pb.x == dx) && (pa.y - pb.y == dy);
        });
}


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
                std::cout << std::accumulate(polygons.begin(), polygons.end(), 0.0,
                    [](double sum, const Polygon& poly)
                    {
                        return poly.points.size() % 2 == 0 ? sum + PolygonArea(poly) : sum;
                    }) << "\n";
            }
            else if(param == "ODD")
            {
                std:: cout << std::accumulate(polygons.begin(), polygons.end(), 0.0,
                    [](double sum, const Polygon& poly)
                    {
                        return poly.points.size() % 2 != 0 ? sum + PolygonArea(poly) : sum;
                    }) << "\n";
            }
            else if(param == "MEAN")
            {
                if(polygons.empty())
                {
                    std::cout << "<INVALID COMMAND>\n";
                }
                else
                {
                    double total = std::accumulate(polygons.begin(), polygons.end(), 0.0,
                        [](double sum, const Polygon& poly)
                        {
                            return sum + PolygonArea(poly);
                        });
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
                        std::cout << std::accumulate(polygons.begin(), polygons.end(), 0.0,
                            [n](double sum, const Polygon& poly)
                            {
                                return poly.points.size() == n ? sum + PolygonArea(poly) : sum;
                            }) << "\n";
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
                ? std::max_element(polygons.begin(), polygons.end(),
                    [](const Polygon& a, const Polygon& b) { return PolygonArea(a) < PolygonArea(b); })

                : std::min_element(polygons.begin(), polygons.end(),
                    [](const Polygon& a, const Polygon& b) { return PolygonArea(a) < PolygonArea(b); });

                std::cout << std::fixed << std::setprecision(1) << PolygonArea(*it) << "\n";
            }
            else if(param == "VERTEXES")
            {
                auto it = (cmd == "MAX")
                ? std::max_element(polygons.begin(), polygons.end(),
                    [](const Polygon& a, const Polygon& b) { return a.points.size() < b.points.size(); })

                : std::min_element(polygons.begin(), polygons.end(),
                    [](const Polygon& a, const Polygon&b) { return a.points.size() < b.points.size(); });

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
                std::cout << std::count_if(polygons.begin(), polygons.end(),
                    [](const Polygon& poly){ return poly.points.size() % 2 == 0; } ) << "\n";
            }
            else if(param == "ODD")
            {
                std::cout << std::count_if(polygons.begin(), polygons.end(),
                    [](const Polygon& poly){ return poly.points.size() % 2 != 0; } ) << "\n";
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
                    std::cout << std::count_if(polygons.begin(), polygons.end(),
                        [n](const Polygon& poly){ return poly.points.size() == n; } ) << "\n";
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
            Polygon target;
            if(!(std::cin >> target))
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                std::cout << "<INVALID COMMAND>\n";
            }
            else
            {
                bool valid = true;
                while(std::cin.peek() != '\n' && std::cin.peek() != EOF)
                {
                    if(!std::isspace(std::cin.peek()))
                    {
                        valid = false;
                        break;
                    }
                    std::cin.get();
                }

                if(valid)
                {
                    std::cout << std::count_if(polygons.begin(), polygons.end(),
                        [&target](const Polygon& poly){ return isSame(poly, target); }) << "\n";
                }
                else
                {
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "<INVALID COMMAND>\n";
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
