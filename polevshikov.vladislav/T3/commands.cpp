#include "commands.h"
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <limits>

bool checkAndConsumeRestOfLine(std::istream& in) {
    while (in.peek() == ' ' || in.peek() == '\t') {
        in.get();
    }
    char next = in.peek();
    if (next == '\n' || next == EOF) {
        if (next == '\n') {
            in.get();
        }
        return true;
    }
    return false;
}

void doArea(const std::vector<Polygon>& polygons, std::istream& in, std::ostream& out) {
    std::string subCmd;
    if (!(in >> subCmd) || !checkAndConsumeRestOfLine(in)) {
        throw std::invalid_argument("extra symbols");
    }

    double res = 0.0;
    if (subCmd == "EVEN") {
        std::vector<Polygon> filtered;

        std::copy_if(polygons.begin(), polygons.end(), std::back_inserter(filtered), isEven);
        res = std::accumulate(filtered.begin(), filtered.end(), 0.0, sumArea);

    } else if (subCmd == "ODD") {
        std::vector<Polygon> filtered;

        std::copy_if(polygons.begin(), polygons.end(), std::back_inserter(filtered), isOdd);
        res = std::accumulate(filtered.begin(), filtered.end(), 0.0, sumArea);

    } else if (subCmd == "MEAN") {
        if (polygons.empty()) {
            throw std::logic_error("Empty collection");
        }

        res = std::accumulate(polygons.begin(), polygons.end(), 0.0, sumArea) / polygons.size();

    } else {
        size_t num = std::stoi(subCmd);
        if (num < 3) {
            throw std::invalid_argument("bad vertex count");
        }
        std::vector<Polygon> filtered;
        std::copy_if(polygons.begin(), polygons.end(), std::back_inserter(filtered), HasNVertices{num});
        res = std::accumulate(filtered.begin(), filtered.end(), 0.0, sumArea);
    }
    out << std::fixed << std::setprecision(1) << res << '\n';
}

void doMax(const std::vector<Polygon>& polygons, std::istream& in, std::ostream& out) {
    if (polygons.empty()) {
        throw std::logic_error("Empty collection");
    }

    std::string subCmd;
    if (!(in >> subCmd) || !checkAndConsumeRestOfLine(in)) {
        throw std::invalid_argument("extra symbols");
    }

    if (subCmd == "AREA") {
        auto it = std::max_element(polygons.begin(), polygons.end(), compareByArea);
        out << std::fixed << std::setprecision(1) << getArea(*it) << '\n';

    } else if (subCmd == "VERTEXES") {
        auto it = std::max_element(polygons.begin(), polygons.end(), compareBySize);
        out << it->points.size() << "\n";

    } else {
        throw std::invalid_argument("bad subcommand");
    }
}

void doMin(const std::vector<Polygon>& polygons, std::istream& in, std::ostream& out) {
    if (polygons.empty()) {
        throw std::logic_error("Empty collection");
    }

    std::string subCmd;
    if (!(in >> subCmd) || !checkAndConsumeRestOfLine(in)) {
        throw std::invalid_argument("extra symbols");
    }

    if (subCmd == "AREA") {
        auto it = std::min_element(polygons.begin(), polygons.end(), compareByArea);
        out << std::fixed << std::setprecision(1) << getArea(*it) << '\n';

    } else if (subCmd == "VERTEXES") {
        auto it = std::min_element(polygons.begin(), polygons.end(), compareBySize);
        out << it->points.size() << "\n";

    } else {
        throw std::invalid_argument("bad subcommand");
    }
}

void doCount(const std::vector<Polygon>& polygons, std::istream& in, std::ostream& out) {
    std::string subCmd;
    if (!(in >> subCmd) || !checkAndConsumeRestOfLine(in)) {
        throw std::invalid_argument("extra symbols");
    }

    size_t cnt = 0;

    if (subCmd == "EVEN") {
        cnt = std::count_if(polygons.begin(), polygons.end(), isEven);

    } else if (subCmd == "ODD") {
        cnt = std::count_if(polygons.begin(), polygons.end(), isOdd);

    } else {
        size_t num = std::stoi(subCmd);
        if (num < 3) {
            throw std::invalid_argument("bad vertex count");
        }
        cnt = std::count_if(polygons.begin(), polygons.end(), HasNVertices{num});
    }
    out << cnt << '\n';
}

void printError(std::ostream& out) {
    out << "<INVALID COMMAND>\n";
}

void doRightShapes(const std::vector<Polygon>& polygons, std::istream& in, std::ostream& out) {
    if (!checkAndConsumeRestOfLine(in)) {
        printError(out);
        return;
    }

    long long count = std::count_if(polygons.begin(), polygons.end(), hasRightAngle);
    out << count << '\n';
}

void doInFrame(const std::vector<Polygon>& polygons, std::istream& in, std::ostream& out) {
    Polygon target;
    if (!(in >> target) || !checkAndConsumeRestOfLine(in)) {
        in.clear();
        in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        printError(out);
        return;
    }

    if (polygons.empty()) {
        throw std::logic_error("Empty collection");
    }

    int minX = std::accumulate(polygons.begin(), polygons.end(), std::numeric_limits<int>::max(), getMinX);
    int maxX = std::accumulate(polygons.begin(), polygons.end(), std::numeric_limits<int>::min(), getMaxX);
    int minY = std::accumulate(polygons.begin(), polygons.end(), std::numeric_limits<int>::max(), getMinY);
    int maxY = std::accumulate(polygons.begin(), polygons.end(), std::numeric_limits<int>::min(), getMaxY);

    bool inInside = std::all_of(target.points.begin(), target.points.end(), [minX, maxX, minY, maxY](const Point& p) {
        return p.x >= minX && p.x <= maxX && p.y >= minY && p.y <= maxY;
    });
    out << (inInside ? "<TRUE>" : "<FALSE>") << '\n';
}
