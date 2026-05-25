#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iterator>
#include <algorithm>
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
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        Polygon poly;
        if (iss>>poly) {
            iss >> std::ws;                    // пропускаем пробельные символы
            if (iss.eof()) {                   // если дошли до конца — строка корректна
                polygons.push_back(std::move(poly));
            }
        }
    }
    processCommands(polygons);
    return 0;
}
