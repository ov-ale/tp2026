#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <functional>
#include <limits>
#include "polygon.h"
#include "commands.h"

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

    cmds["AREA"] = [&](std::istream& in, std::ostream& out) {doArea(polygons, in, out);};
    cmds["MAX"] = [&](std::istream& in, std::ostream& out) {doMax(polygons, in, out);};
    cmds["MIN"] = [&](std::istream& in, std::ostream& out) {doMin(polygons, in, out);};
    cmds["COUNT"] = [&](std::istream& in, std::ostream& out) {doCount(polygons, in, out); };
    cmds["RIGHTSHAPES"] = [&](std::istream&, std::ostream& out) {doRightShapes(polygons, out); };
    cmds["INFRAME"] = [&](std::istream& in, std::ostream& out) {doInFrame(polygons, in, out); };

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
