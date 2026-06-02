#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <functional>
#include <limits>
#include "polygon.h"
#include "commands.h"

using namespace std::placeholders;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>\n";
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Cannot open file: " << argv[1] << '\n';
        return 1;
    }

    std::vector<Polygon> polygons;

    while (file) {
        Polygon p;
        if (file >> p) {
            polygons.push_back(p);
        } else if (!file.eof()) {
            file.clear();
            file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    std::map<std::string, std::function<void(std::istream&, std::ostream&)>> cmds;

    cmds["AREA"] = std::bind(doArea, std::cref(polygons), _1, _2);
    cmds["MAX"] = std::bind(doMax, std::cref(polygons), _1, _2);
    cmds["MIN"] = std::bind(doMin, std::cref(polygons), _1, _2);
    cmds["COUNT"] = std::bind(doCount, std::cref(polygons), _1, _2);
    cmds["RIGHTSHAPES"] = std::bind(doRightShapes, std::cref(polygons), _1, _2);
    cmds["INFRAME"] = std::bind(doInFrame, std::cref(polygons), _1, _2);

    std::string cmd;
    while(std::cin >> cmd) {
        try {
            if (cmds.count(cmd)) {
                cmds[cmd](std::cin, std::cout);
            } else {
                printError(std::cout);
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }

        } catch (...) {
            printError(std::cout);
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
    return 0;
}
