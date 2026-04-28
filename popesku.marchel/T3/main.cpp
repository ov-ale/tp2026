#include <algorithm>
#include <cmath>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>


struct Point {
    int x, y;
};
struct Polygon {
    std::vector<Point> points;
};

bool operator==(const Point& lhs, const Point& rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

bool operator==(const Polygon& lhs, const Polygon& rhs)
{
    return lhs.points == rhs.points;
}

std::istream& operator>>(std::istream& in, Point& point) { // (x;y) - читать вот это
    char leftBracket = 0;
    char semicolon = 0;
    char rightBracket = 0;

    int x = 0;
    int y = 0;

    in >> leftBracket >> x >> semicolon >> y >> rightBracket;

    if (!in || leftBracket != '(' || semicolon != ';' || rightBracket != ')') {
        in.setstate(std::ios::failbit);
        return in;
    }
    point = Point{x,y};
    return in;
}

std::istream& operator>>(std::istream& in, Polygon& polygon)
{
    std::size_t vertexCount = 0;
    in >> vertexCount;

    if (!in || vertexCount < 3)
    {
        in.setstate(std::ios::failbit);
        return in;
    }

    std::vector<Point> points;
    points.reserve(vertexCount);

    std::copy_n(
      std::istream_iterator<Point>(in),
      vertexCount,
      std::back_inserter(points)
    );

    if (!in || points.size() != vertexCount)
    {
        in.setstate(std::ios::failbit);
        return in;
    }

    polygon.points = points;
    return in;
}

std::vector<Polygon> readPolygonsFromFile(const std::string& filename)
{
    std::ifstream file(filename);

    if (!file)
    {
        throw std::runtime_error("Cannot open file");
    }

    std::vector<Polygon> polygons;
    std::string line;

    while (std::getline(file, line))
    {
        std::istringstream input(line);

        Polygon polygon;
        std::string extra;

        if ((input >> polygon) && !(input >> extra))
        {
            polygons.push_back(polygon);
        }
    }

    return polygons;
}

struct CrossProduct
{
    long long operator()(const Point& lhs, const Point& rhs) const
    {
        return static_cast<long long>(lhs.x) * rhs.y -
               static_cast<long long>(lhs.y) * rhs.x;
    }
};

double getArea(const Polygon& polygon)
{
    if (polygon.points.size() < 3)
    {
        return 0.0;
    }

    std::vector<Point> shiftedPoints(polygon.points.begin() + 1, polygon.points.end());
    shiftedPoints.push_back(polygon.points.front());

    long long doubledArea = std::inner_product(
      polygon.points.begin(),
      polygon.points.end(),
      shiftedPoints.begin(),
      0LL,
      std::plus<long long>(),
      CrossProduct{}
    );

    return std::abs(doubledArea) / 2.0;
}

struct IsEvenVertexCount
{
    bool operator()(const Polygon& polygon) const
    {
        return polygon.points.size() % 2 == 0;
    }
};

struct IsOddVertexCount
{
    bool operator()(const Polygon& polygon) const
    {
        return polygon.points.size() % 2 != 0;
    }
};

struct HasVertexCount
{
    std::size_t count;

    bool operator()(const Polygon& polygon) const
    {
        return polygon.points.size() == count;
    }
};

template <typename Predicate>
struct AreaSumIf
{
    Predicate predicate;

    double operator()(double sum, const Polygon& polygon) const
    {
        if (predicate(polygon))
        {
            return sum + getArea(polygon);
        }

        return sum;
    }
};

template <typename Predicate>
double getAreaSumIf(const std::vector<Polygon>& polygons, Predicate predicate)
{
    return std::accumulate(
      polygons.begin(),
      polygons.end(),
      0.0,
      AreaSumIf<Predicate>{predicate}
    );
}

struct AreaSum
{
    double operator()(double sum, const Polygon& polygon) const
    {
        return sum + getArea(polygon);
    }
};

double getAreaMean(const std::vector<Polygon>& polygons)
{
    if (polygons.empty())
    {
        throw std::logic_error("empty collection");
    }

    double sum = std::accumulate(
      polygons.begin(),
      polygons.end(),
      0.0,
      AreaSum{}
    );

    return sum / polygons.size();
}

void checkNoExtraArguments(std::istream& in)
{
    std::string extra;
    if (in >> extra)
    {
        throw std::logic_error("invalid command");
    }
}

void processAreaCommand(std::istream& in, const std::vector<Polygon>& polygons)
{
    std::string argument;
    in >> argument;

    if (!in)
    {
        throw std::logic_error("invalid command");
    }

    if (argument == "EVEN")
    {
        checkNoExtraArguments(in);
        std::cout << getAreaSumIf(polygons, IsEvenVertexCount{}) << '\n';
    }
    else if (argument == "ODD")
    {
        checkNoExtraArguments(in);
        std::cout << getAreaSumIf(polygons, IsOddVertexCount{}) << '\n';
    }
    else if (argument == "MEAN")
    {
        checkNoExtraArguments(in);
        std::cout << getAreaMean(polygons) << '\n';
    }
    else
    {
        std::istringstream numberInput(argument);

        std::size_t vertexCount = 0;
        numberInput >> vertexCount;

        std::string extraInNumber;
        if (!numberInput || vertexCount < 3 || numberInput >> extraInNumber)
        {
            throw std::logic_error("invalid command");
        }

        checkNoExtraArguments(in);
        std::cout << getAreaSumIf(polygons, HasVertexCount{vertexCount}) << '\n';
    }
}

void processCountCommand(std::istream& in, const std::vector<Polygon>& polygons)
{
    std::string argument;
    in >> argument;

    if (!in)
    {
        throw std::logic_error("invalid command");
    }

    if (argument == "EVEN")
    {
        checkNoExtraArguments(in);

        std::cout << std::count_if(
          polygons.begin(),
          polygons.end(),
          IsEvenVertexCount{}
        ) << '\n';
    }
    else if (argument == "ODD")
    {
        checkNoExtraArguments(in);

        std::cout << std::count_if(
          polygons.begin(),
          polygons.end(),
          IsOddVertexCount{}
        ) << '\n';
    }
    else
    {
        std::istringstream numberInput(argument);

        std::size_t vertexCount = 0;
        numberInput >> vertexCount;

        std::string extraInNumber;
        if (!numberInput || vertexCount < 3 || numberInput >> extraInNumber)
        {
            throw std::logic_error("invalid command");
        }

        checkNoExtraArguments(in);

        std::cout << std::count_if(
          polygons.begin(),
          polygons.end(),
          HasVertexCount{vertexCount}
        ) << '\n';
    }
}

struct CompareArea
{
    bool operator()(const Polygon& lhs, const Polygon& rhs) const
    {
        return getArea(lhs) < getArea(rhs);
    }
};

struct CompareVertexCount
{
    bool operator()(const Polygon& lhs, const Polygon& rhs) const
    {
        return lhs.points.size() < rhs.points.size();
    }
};

double getMaxArea(const std::vector<Polygon>& polygons)
{
    if (polygons.empty())
    {
        throw std::logic_error("empty collection");
    }

    auto result = std::max_element(
      polygons.begin(),
      polygons.end(),
      CompareArea{}
    );

    return getArea(*result);
}

std::size_t getMaxVertexCount(const std::vector<Polygon>& polygons)
{
    if (polygons.empty())
    {
        throw std::logic_error("empty collection");
    }

    auto result = std::max_element(
      polygons.begin(),
      polygons.end(),
      CompareVertexCount{}
    );

    return result->points.size();
}

double getMinArea(const std::vector<Polygon>& polygons)
{
    if (polygons.empty())
    {
        throw std::logic_error("empty collection");
    }

    auto result = std::min_element(
      polygons.begin(),
      polygons.end(),
      CompareArea{}
    );

    return getArea(*result);
}

std::size_t getMinVertexCount(const std::vector<Polygon>& polygons)
{
    if (polygons.empty())
    {
        throw std::logic_error("empty collection");
    }

    auto result = std::min_element(
      polygons.begin(),
      polygons.end(),
      CompareVertexCount{}
    );

    return result->points.size();
}

void processMaxCommand(std::istream& in, const std::vector<Polygon>& polygons)
{
    std::string argument;
    in >> argument;

    if (!in)
    {
        throw std::logic_error("invalid command");
    }

    checkNoExtraArguments(in);

    if (argument == "AREA")
    {
        std::cout << getMaxArea(polygons) << '\n';
    }
    else if (argument == "VERTEXES")
    {
        std::cout << getMaxVertexCount(polygons) << '\n';
    }
    else
    {
        throw std::logic_error("invalid command");
    }
}

void processMinCommand(std::istream& in, const std::vector<Polygon>& polygons)
{
    std::string argument;
    in >> argument;

    if (!in)
    {
        throw std::logic_error("invalid command");
    }

    checkNoExtraArguments(in);

    if (argument == "AREA")
    {
        std::cout << getMinArea(polygons) << '\n';
    }
    else if (argument == "VERTEXES")
    {
        std::cout << getMinVertexCount(polygons) << '\n';
    }
    else
    {
        throw std::logic_error("invalid command");
    }
}

long long getDotProduct(const Point& previous, const Point& current, const Point& next)
{
    long long x1 = previous.x - current.x;
    long long y1 = previous.y - current.y;

    long long x2 = next.x - current.x;
    long long y2 = next.y - current.y;

    return x1 * x2 + y1 * y2;
}

struct IsRightAngleAt
{
    const std::vector<Point>& points;

    bool operator()(std::size_t index) const
    {
        std::size_t size = points.size();

        const Point& previous = points[(index + size - 1) % size];
        const Point& current = points[index];
        const Point& next = points[(index + 1) % size];

        return getDotProduct(previous, current, next) == 0;
    }
};

bool isRectangle(const Polygon& polygon)
{
    if (polygon.points.size() != 4)
    {
        return false;
    }

    std::array<std::size_t, 4> indexes = {0, 1, 2, 3};

    return std::all_of(
      indexes.begin(),
      indexes.end(),
      IsRightAngleAt{polygon.points}
    );
}

struct IsRectangle
{
    bool operator()(const Polygon& polygon) const
    {
        return isRectangle(polygon);
    }
};

void processRectsCommand(std::istream& in, const std::vector<Polygon>& polygons)
{
    checkNoExtraArguments(in);

    std::cout << std::count_if(
      polygons.begin(),
      polygons.end(),
      IsRectangle{}
    ) << '\n';
}

struct HasSameShift
{
    int shiftX;
    int shiftY;

    bool operator()(const Point& lhs, const Point& rhs) const
    {
        return rhs.x - lhs.x == shiftX && rhs.y - lhs.y == shiftY;
    }
};

bool isSameByTranslation(const Polygon& lhs, const Polygon& rhs)
{
    if (lhs.points.size() != rhs.points.size())
    {
        return false;
    }

    int shiftX = rhs.points[0].x - lhs.points[0].x;
    int shiftY = rhs.points[0].y - lhs.points[0].y;

    return std::equal(
      lhs.points.begin(),
      lhs.points.end(),
      rhs.points.begin(),
      HasSameShift{shiftX, shiftY}
    );
}

struct IsSameByTranslation
{
    Polygon sample;

    bool operator()(const Polygon& polygon) const
    {
        return isSameByTranslation(polygon, sample);
    }
};

void processSameCommand(std::istream& in, const std::vector<Polygon>& polygons)
{
    Polygon target;
    in >> target;

    if (!in)
    {
        throw std::logic_error("invalid command");
    }

    checkNoExtraArguments(in);

    std::cout << std::count_if(
      polygons.begin(),
      polygons.end(),
      IsSameByTranslation{target}
    ) << '\n';
}







int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Filename is not specified\n";
        return 1;
    }

    std::vector<Polygon> polygons;

    try
    {
        polygons = readPolygonsFromFile(argv[1]);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }

    std::cout << std::fixed << std::setprecision(1);

    std::string line;

    while (std::getline(std::cin, line))
    {
        std::istringstream input(line);

        std::string command;
        input >> command;

        try
        {
            if (!input)
            {
                throw std::logic_error("invalid command");
            }

            if (command == "AREA")
            {
                processAreaCommand(input, polygons);
            }
            else if (command == "COUNT")
            {
                processCountCommand(input, polygons);
            }
            else if (command == "MAX")
            {
                processMaxCommand(input, polygons);
            }
            else if (command == "MIN")
            {
                processMinCommand(input, polygons);
            }
            else if (command == "RECTS")
            {
                processRectsCommand(input, polygons);
            }
            else if (command == "SAME")
            {
                processSameCommand(input, polygons);
            }
            else
            {
                throw std::logic_error("invalid command");
            }
        }
        catch (const std::logic_error&)
        {
            std::cout << "<INVALID COMMAND>\n";
        }
    }

    return 0;
}
