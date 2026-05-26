#include "datastruct.h"
#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <sstream>

int main() {
    std::vector<DataStruct> data;
    
    // Тестовые данные прямо в коде
    std::string testData = 
        "(:key1 0ull:key2 0b0:key3 \"Data\":)\n"
        "(:key1 89ull:key2 0b0:key3 \"Data\":)\n"
        "(:key1 \"Data\":key2 \"Data\":key3 \"Data\":)\n"
        "(:key1 89ull:key2 0b1:key3 \"Data\":)\n"
        "(:key1 89ull:key2 0b1:key3 \"Data with :\":)\n";
    
    std::istringstream input(testData);
    std::string line;
    
    while (std::getline(input, line)) {
        if (line.empty()) continue;
        
        std::istringstream iss(line);
        DataStruct ds;
        
        if (iss >> ds) {
            data.push_back(ds);
        }
        // Если ошибка - просто пропускаем строку
    }
    
    // Сортировка
    std::sort(data.begin(), data.end(), compareDataStruct);
    
    // Вывод
    std::copy(
        data.begin(),
        data.end(),
        std::ostream_iterator<DataStruct>(std::cout, "\n")
    );
    
    return 0;
}