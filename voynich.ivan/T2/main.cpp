#include "datastruct.h"
#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <sstream>

int main() {
    std::vector<DataStruct> data;
    std::string line;

    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;

        std::istringstream iss(line);
        DataStruct ds;
        if (iss >> ds) {
            data.push_back(ds);
        }
    }

    std::sort(data.begin(), data.end(), compareDataStruct);

    std::copy(
        data.begin(),
        data.end(),
        std::ostream_iterator<DataStruct>(std::cout, "\n")
    );

    return 0;
}

