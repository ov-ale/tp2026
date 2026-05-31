#include "commands.h"
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <limits>

void doArea(const std::vector<Polygon>& polygons, std::istream& in, std::ostream& out) {
    std::string subCmd;
    in >> subCmd;

    double res = 0.0;
    if (subCmd == "EVEN") {
        res = std::accumulate(polygons.begin(), polygons.end(), 0.0, [](double sum, const Polygon& p) {
            return (p.points.size() % 2 == 0) ? sum + getArea(p) : sum;
        });

    } else if (subCmd == "ODD") {
        res = std::accumulate(polygons.begin(), polygons.end(), 0.0, [](double sum, const Polygon& p) {
            return (p.points.size() % 2 != 0) ? sum + getArea(p) : sum;
        });

    } else if (subCmd == "MEAN") {
        if (polygons.empty()) {
            throw std::logic_error("Empty collection");
        }

        double totalArea = std::accumulate(polygons.begin(), polygons.end(), 0.0, [](double sum, const Polygon&p) {
            return sum + getArea(p);
        });

        res = totalArea / polygons.size();

    } else {
        size_t num = std::stoi(subCmd);
        res = std::accumulate(polygons.begin(), polygons.end(), 0.0, [num](double sum, const Polygon& p) {
            return (p.points.size() == num) ? sum + getArea(p) : sum;
        });
    }
    out << std::fixed << std::setprecision(1) << res << '\n';
}

void doMax(const std::vector<Polygon>& polygons, std::istream& in, std::ostream& out) {
    if (polygons.empty()) {
        throw std::logic_error("Empty collection");
    }

    std::string subCmd;
    in >> subCmd;

    if (subCmd == "AREA") {
        auto it = std::max_element(polygons.begin(), polygons.end(), [](const Polygon& a, const Polygon& b) {
            return getArea(a) < getArea(b);
        });
        out << std::fixed << std::setprecision(1) << getArea(*it) << '\n';

    } else if (subCmd == "VERTICES") {
        auto it = std::max_element(polygons.begin(), polygons.end(), [](const Polygon& a, const Polygon& b) {
            return a.points.size() < b.points.size();
        });
        out << it->points.size() << "\n";
    }
}

void doMin(const std::vector<Polygon>& polygons, std::istream& in, std::ostream& out) {
    if (polygons.empty()) {
        throw std::logic_error("Empty collection");
    }

    std::string subCmd;
    in >> subCmd;

    if (subCmd == "AREA") {
        auto it = std::min_element(polygons.begin(), polygons.end(), [](const Polygon& a, const Polygon& b) {
            return getArea(a) < getArea(b);
        });
        out << std::fixed << std::setprecision(1) << getArea(*it) << '\n';

    } else if (subCmd == "VERTICES") {
        auto it = std::min_element(polygons.begin(), polygons.end(), [](const Polygon& a, const Polygon& b) {
            return a.points.size() < b.points.size();
        });
        out << it->points.size() << "\n";
    }
}

void doCount(const std::vector<Polygon>& polygons, std::istream& in, std::ostream& out) {
    std::string subCmd;
    in >> subCmd;

    size_t cnt = 0;

    if (subCmd == "EVEN") {
        cnt = std::count_if(polygons.begin(), polygons.end(), [](const Polygon& p) {
            return p.points.size() % 2 == 0;
        });

    } else if (subCmd == "ODD") {
        cnt = std::count_if(polygons.begin(), polygons.end(), [](const Polygon& p) {
            return p.points.size() % 2 != 0;
        });

    } else {
        size_t num = std::stoi(subCmd);
        cnt = std::count_if(polygons.begin(), polygons.end(), [num](const Polygon& p) {
            return p.points.size() == num;
        });
    }
    out << cnt << '\n';
}

void printError(std::ostream& out) {
    out << "<INVALID COMMAND>\n";
}

void doRightShapes(const std::vector<Polygon>& polygons, std::ostream& out) {
    long long count = std::count_if(polygons.begin(), polygons.end(), hasRightAngle);
    out << count << '\n';
}

void doInFrame(const std::vector<Polygon>& polygons, std::istream& in, std::ostream& out) {
    Polygon target;
    if (!(in >> target)) {
        printError(out);
        return;
    }

    if (polygons.empty()) {
        out << "<FALSE>\n";
        return;
    }

    auto findMinX = [](int cur, const Polygon& p) {
        auto it = std::min_element(p.points.begin(), p.points.end(), [](const Point& a, const Point& b) {
            return a.x < b.x;
        });
        return std::min(cur, it->x);
    };

    auto findMaxX = [](int cur, const Polygon& p) {
        auto it = std::max_element(p.points.begin(), p.points.end(), [](const Point& a, const Point& b) {
            return a.x < b.x;
        });
        return std::max(cur, it->x);
    };

    auto findMinY = [](int cur, const Polygon& p) {
        auto it = std::min_element(p.points.begin(), p.points.end(), [](const Point& a, const Point& b) {
            return a.y < b.y;
        });
        return std::min(cur, it->y);
    };

    auto findMaxY = [](int cur, const Polygon& p) {
        auto it = std::max_element(p.points.begin(), p.points.end(), [](const Point& a, const Point& b) {
            return a.y < b.y;
        });
        return std::max(cur, it->y);
    };

    int minX = std::accumulate(polygons.begin(), polygons.end(), std::numeric_limits<int>::max(), findMinX);
    int maxX = std::accumulate(polygons.begin(), polygons.end(), std::numeric_limits<int>::min(), findMaxX);
    int minY = std::accumulate(polygons.begin(), polygons.end(), std::numeric_limits<int>::max(), findMinY);
    int maxY = std::accumulate(polygons.begin(), polygons.end(), std::numeric_limits<int>::min(), findMaxY);

    bool inInside = std::all_of(target.points.begin(), target.points.end(), [minX, maxX, minY, maxY](const Point& p) {
        return p.x >= minX && p.x <= maxX && p.y >= minY && p.y <= maxY;
    });
    out << (inInside ? "<TRUE>" : "<FALSE>") << '\n';
}
