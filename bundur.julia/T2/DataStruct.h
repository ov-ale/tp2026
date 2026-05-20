#ifndef DATA_STRUCT_H
#define DATA_STRUCT_H
#include <iostream>
#include <string>
#include <vector>
struct DataStruct {
    double key1;
    unsigned long long key2;
    std::string key3;
};
bool compareData(const DataStruct& lhs, const DataStruct& rhs);
std::istream& operator>>(std::istream& in, DataStruct& dest);
std::ostream& operator<<(std::ostream& out, const DataStruct& src);
struct DelimiterIO { 
    char exp; 
};
struct DoubleLitIO { 
    double& ref; 
};
struct UllLitIO { 
    unsigned long long& ref; 
};
struct StringIO { 
    std::string& ref; 
};
std::istream& operator>>(std::istream& in, DelimiterIO&& dest);
std::istream& operator>>(std::istream& in, DoubleLitIO&& dest);
std::istream& operator>>(std::istream& in, UllLitIO&& dest);
std::istream& operator>>(std::istream& in, StringIO&& dest);
#endif
