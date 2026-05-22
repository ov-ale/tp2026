#include <iostream>
#include <fstream>
#include <vector>
#include <limits>
#include "Geom.h"
#include "Command.h"
int main(int argc, char* argv[]) {
    if (argc!=2) {
        std::cerr<<"Error: No filename provided\n";
        return 1;
    }
    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr<<"Error: Could not open file\n";
        return 1;
    }
    std::vector<Polygon> polygons;
    while (!file.eof()) {
        Polygon poly;
        if (file>>poly) {
            polygons.push_back(poly);
        }
        else {
            file.clear();
            file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
    file.close();
    processCommands(polygons);
    return 0;
}
