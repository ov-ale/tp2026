#ifndef DATA_STRUCT_HPP
#define DATA_STRUCT_HPP
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
struct LabelIO {
    std::string exp;
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
std::istream& operator>>(std::istream& in, LabelIO&& dest);
std::istream& operator>>(std::istream& in, DoubleLitIO&& dest);
std::istream& operator>>(std::istream& in, UllLitIO&& dest);
std::istream& operator>>(std::istream& in, StringIO&& dest);
#endif
