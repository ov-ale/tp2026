#include <iostream>
#include <string>

int main() {
    std::string line;
    bool hasAny = false;
    
    while (std::getline(std::cin, line)) {
        if (line.find("0ull") != std::string::npos || 
            line.find("0b0") != std::string::npos) {
            hasAny = true;
        }
    }
    
    if (!hasAny) {
        std::cout << "Looks like there is no supported record. Cannot determine input. Test skipped" << std::endl;
        return 0;
    }
    
    std::cout << "Atleast one supported record type" << std::endl;
    return 1;
}
