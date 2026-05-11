#include <iostream>
#include <functional>
#include <iterator>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>
#include <numeric>
#include <cstdlib>
#include <cstddef>
#include <limits>
#include <iomanip>

struct Point
{
    int x, y;

    bool operator==(const Point& other) const
    {
        return (this->x == other.x) && (this->y == other.y);
    }
};

struct Polygon
{
    std::vector<Point> points;

    bool operator==(const Polygon& other) const
    {
        return this->points == other.points;
    }
};

struct AreaCalculator
{
    const std::vector<Point>& pts;
    std::size_t size;
    mutable std::size_t idx;

    explicit AreaCalculator(const std::vector<Point>& pts) : pts(pts), size(pts.size()), idx(0)
    {
    }

    long long operator()(long long acc, const Point& curr) const
    {
        const Point& next = pts[(idx + 1) % size];
        long long term = static_cast<long long>(curr.x) * next.y - static_cast<long long>(next.x) * curr.y;
        ++idx;
        return acc + term;
    }
};

double getArea(const Polygon& p)
{
    if (p.points.size() < 3) return 0.0;
    long long sum = std::accumulate (p.points.begin(), p.points.end(), 0LL, AreaCalculator(p.points));
    return std::abs(sum) / 2.0;
}

struct SumAreaIfEven
{
    double operator()(double acc, const Polygon& p) const
    {
        return (p.points.size() % 2 == 0) ? acc + getArea(p) : acc;
    }
};
struct SumAreaIfOdd
{
    double operator()(double acc, const Polygon& p) const
    {
        return (p.points.size() % 2 != 0) ? acc + getArea(p) : acc;
    }
};
struct SumArea
{
    double operator()(double acc, const Polygon& p) const
    {
        return acc + getArea(p);
    }
};

struct SumAreaByVertexes
{
    std::size_t verx;
    explicit SumAreaByVertexes(std::size_t verx_) : verx(verx_)
    {
    }

    double operator()(double acc, const Polygon& p) const
    {
        return (p.points.size() == verx) ? acc + getArea(p) : acc;
    }
};

struct CompareByArea
{
    bool operator()(const Polygon& a, const Polygon& b) const
    {
        return getArea(a) < getArea(b);
    }
};

struct CompareByVertexCount
{
    bool operator()(const Polygon& a, const Polygon& b) const
    {
        return a.points.size() < b.points.size();
    }
};

struct SortPoints
{
    bool operator()(const Point& a, const Point& b) const
    {
        if (a.x == b.x) return a.y < b.y;
        return a.x < b.x;
    }
};

int getMinX(const std::vector<Polygon>& polygons)
{
    auto it = std::min_element(polygons.begin(), polygons.end(),
        [](const Polygon& a, const Polygon& b)
        {
            int min_x_a = std::min_element(a.points.begin(), a.points.end(),
                [](const Point& pa, const Point& pb) { return pa.x < pb.x; })->x;
            int min_x_b = std::min_element(b.points.begin(), b.points.end(),
                [](const Point& pa, const Point& pb) { return pa.x < pb.x; })->x;
            return min_x_a < min_x_b;
        });

    return std::min_element(it->points.begin(), it->points.end(),
        [](const Point& pa, const Point& pb) { return pa.x < pb.x; })->x;
}

int getMinY(const std::vector<Polygon>& polygons)
{
    auto it = std::min_element(polygons.begin(), polygons.end(),
        [](const Polygon& a, const Polygon& b)
        {
            int min_y_a = std::min_element(a.points.begin(), a.points.end(),
                [](const Point& pa, const Point& pb) { return pa.y < pb.y; })->y;
            int min_y_b = std::min_element(b.points.begin(), b.points.end(),
                [](const Point& pa, const Point& pb) { return pa.y < pb.y; })->y;
            return min_y_a < min_y_b;
        });

    return std::min_element(it->points.begin(), it->points.end(),
        [](const Point& pa, const Point& pb) { return pa.y < pb.y; })->y;
}

int getMaxX(const std::vector<Polygon>& polygons)
{
    auto it = std::max_element(polygons.begin(), polygons.end(),
        [](const Polygon& a, const Polygon& b)
        {
            int max_x_a = std::max_element(a.points.begin(), a.points.end(),
                [](const Point& pa, const Point& pb) { return pa.x < pb.x; })->x;
            int max_x_b = std::max_element(b.points.begin(), b.points.end(),
                [](const Point& pa, const Point& pb) { return pa.x < pb.x; })->x;
            return max_x_a < max_x_b;
        });

    return std::max_element(it->points.begin(), it->points.end(),
        [](const Point& pa, const Point& pb) { return pa.x < pb.x; })->x;
}

int getMaxY(const std::vector<Polygon>& polygons)
{
    auto it = std::max_element(polygons.begin(), polygons.end(),
        [](const Polygon& a, const Polygon& b)
        {
            int max_y_a = std::max_element(a.points.begin(), a.points.end(),
                [](const Point& pa, const Point& pb) { return pa.y < pb.y; })->y;
            int max_y_b = std::max_element(b.points.begin(), b.points.end(),
                [](const Point& pa, const Point& pb) { return pa.y < pb.y; })->y;
            return max_y_a < max_y_b;
        });

    return std::max_element(it->points.begin(), it->points.end(),
        [](const Point& pa, const Point& pb) { return pa.y < pb.y; })->y;
}

std::istream& operator>>(std::istream& in, Point& p)
{
    char c1, c2, c3;
    if (!(in >> c1 >> p.x >> c2 >> p.y >> c3) || c1 != '(' || c2 != ';' || c3 != ')')
    {
        in.setstate(std::ios_base::failbit);
    }
    return in;
}

std::istream& operator>>(std::istream& in, Polygon& pol)
{
    size_t size{};
    if (!(in >> size) || size < 3)
    {
        in.setstate(std::ios_base::failbit);
        return in;
    }

    std::vector<Point> temp;
    temp.reserve(size);

    std::copy_n(std::istream_iterator<Point>(in), size, std::back_inserter(temp));

    if (temp.size() != size)
    {
        in.setstate(std::ios_base::failbit);
        return in;
    }

    pol.points = std::move(temp);
    return in;
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>\n";
        return EXIT_FAILURE;
    }

    std::ifstream input(argv[1]);
    if (!input)
    {
        std::cerr << "Could not open input file\n";
        return EXIT_FAILURE;
    }

    std::vector<Polygon> polygons;
    std::string line;
    while (std::getline(input, line))
    {
        if (line.empty()) continue;
        std::istringstream iss(line);
        Polygon p;
        if (iss >> p) {
            iss >> std::ws;
            if (iss.eof())
            {
                polygons.push_back(p);
            }
        }
    }

    input.close();
    size_t polySize = polygons.size();

    std::istream& in = std::cin;

    while (std::getline(in, line))
    {
        if (line.empty()) continue;
        std::istringstream iss(line);
        std::string command;
        if (!(iss >> command)) continue;

        try {
            if (command == "AREA")
            {
                std::string arg;
                if (!(iss >> arg)) throw std::invalid_argument("");
                if (arg == "EVEN")
                {
                    std::cout << std::fixed << std::setprecision(1)
                        << std::accumulate(polygons.begin(), polygons.end(), 0.0, SumAreaIfEven()) << "\n";
                }
                else if (arg == "ODD")
                {
                    std::cout << std::fixed << std::setprecision(1)
                        << std::accumulate(polygons.begin(), polygons.end(), 0.0, SumAreaIfOdd()) << "\n";
                }
                else if (arg == "MEAN")
                {
                    if (polygons.empty()) throw std::invalid_argument("");
                    double sum = std::accumulate(polygons.begin(), polygons.end(), 0.0, SumArea());
                    std::cout << std::fixed << std::setprecision(1) << sum / polySize << "\n";
                }
                else
                {
                    try
                    {
                        size_t n = std::stoull(arg);
                        if (n < 3) throw std::invalid_argument("");
                        std::cout << std::fixed << std::setprecision(1)
                            << std::accumulate(polygons.begin(), polygons.end(), 0.0, SumAreaByVertexes(n)) << "\n";
                    }
                    catch (...)
                    {
                        throw std::invalid_argument("");
                    }
                }
            }
            else if (command == "MAX")
            {
                if (polygons.empty()) throw std::invalid_argument("");
                std::string arg;
                if (!(iss >> arg)) throw std::invalid_argument("");
                if (arg == "AREA")
                {
                    auto it = std::max_element(polygons.begin(), polygons.end(), CompareByArea());
                    std::cout << std::fixed << std::setprecision(1) << getArea(*it) << "\n";
                }
                else if (arg == "VERTEX")
                {
                    auto it = std::max_element(polygons.begin(), polygons.end(), CompareByVertexCount());
                    std::cout << it->points.size() << "\n";
                }
                else throw std::invalid_argument("");
            }
            else if (command == "MIN")
            {
                if (polygons.empty()) throw std::invalid_argument("");
                std::string arg;
                if (!(iss >> arg)) throw std::invalid_argument("");
                if (arg == "AREA")
                {
                    auto it = std::min_element(polygons.begin(), polygons.end(), CompareByArea());
                    std::cout << std::fixed << std::setprecision(1) << getArea(*it) << "\n";
                }
                else if (arg == "VERTEX")
                {
                    auto it = std::min_element(polygons.begin(), polygons.end(), CompareByVertexCount());
                    std::cout << it->points.size() << "\n";
                }
            }
            else if (command == "COUNT")
            {
                if (polygons.empty())
                {
                    std::cout << 0 << '\n';
                    continue;
                }
                std::string arg;
                if (!(iss >> arg)) throw std::invalid_argument("");
                if (arg == "EVEN")
                {
                    int count = std::accumulate(polygons.begin(), polygons.end(), 0,
                    [](int acc, Polygon p)
                        {
                            return (p.points.size() % 2 == 0) ? acc + 1 : acc;
                        }
                    );
                    std::cout << count << '\n';
                }
                else if (arg == "ODD")
                {
                    int count = std::accumulate(polygons.begin(), polygons.end(), 0,
                        [](int acc, Polygon p)
                        {
                            return (p.points.size() % 2 == 0) ? acc : acc + 1;
                        });
                    std::cout << count << '\n';
                }
                else
                {
                    try
                    {
                        size_t n = std::stoull(arg);
                        if (n < 3) throw std::invalid_argument("");

                        int count = std::accumulate(polygons.begin(), polygons.end(), 0,
                            [&n](int acc, Polygon& p)
                            {
                                return (p.points.size() == n) ? acc + 1 : acc;
                            }
                        );

                        std::cout << count << '\n';
                    }
                    catch (...)
                    {
                        throw std::invalid_argument("");
                    }
                }
            }
            else if (command == "PERMS")
            {
                try
                {
                    Polygon target;
                    if (!(iss >> target)) throw std::invalid_argument("");

                    iss >> std::ws;
                    if (polygons.empty())
                    {
                        std::cout << 0 << '\n';
                        continue;
                    }
                    if (!iss.eof()) throw std::invalid_argument("");
                    std::sort(target.points.begin(), target.points.end(), SortPoints());

                    size_t result = std::accumulate(polygons.begin(), polygons.end(), 0,
                        [&target](int acc, const Polygon& p)
                        {
                            std::sort(p.points.begin(), p.points.end(), SortPoints());
                            return (p == target) ? acc + 1 : acc;
                        }
                    );
                    std::cout << result << "\n";
                }
                catch (...)
                {
                    throw std::invalid_argument("");
                }
            }
            else if (command == "INFRAME")
            {
                try
                {
                    Polygon target;
                    if (!(iss >> target)) throw std::invalid_argument("");

                    iss >> std::ws;
                    if (!iss.eof()) throw std::invalid_argument("");
                    if (polygons.empty())
                    {
                        std::cout << 0 << '\n';
                        continue;
                    }
                    auto minX = getMinX(polygons);
                    auto minY = getMinY(polygons);
                    auto maxX = getMaxX(polygons);
                    auto maxY = getMaxY(polygons);
                    int count = std::count_if(target.points.begin(), target.points.end(),
                        [&minX, &minY, &maxX, &maxY](Point pt)
                        {
                            return (pt.x > maxX || pt.x < minX || pt.y > maxY || pt.y < minY);
                        });

                    bool ifInframe = (count == 0);
                    std::cout << ifInframe << "\n";
                }
                catch (...)
                {
                    throw std::invalid_argument("");
                }
            }
            else
            {
                throw std::invalid_argument("");
            }

            iss >> std::ws;
            if (!iss.eof())
            {
                throw std::invalid_argument("");
            }
        }
        catch (...)
        {
            std::cout << "<INVALID COMMAND>\n";
        }
    }

    return EXIT_SUCCESS;
}