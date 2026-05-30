#ifndef DATA_STRUCT_H
#define DATA_STRUCT_H
#include <iostream>
#include <string>

struct DataStruct{
    unsigned long long key1 = 0;
    unsigned long long key2 = 0;
    std::string key3;
};

struct DelimiterIO
{
    char exp;
};

struct UllLitIO{
    unsigned long long& ref;
};

struct UllBinIO{
    unsigned long long& ref;
};

struct StringIO{
    std::string& ref;
};


std::string toBinary(unsigned long long value);
bool compareDataStruct(const DataStruct& firstData, const DataStruct& secondData);
std::istream& operator>>(std::istream& in, DelimiterIO&& dest);
std::istream& operator>>(std::istream& in, UllBinIO&& dest);
std::istream& operator>>(std::istream& in, StringIO&& dest);
std::istream& operator>>(std::istream& in, UllLitIO&& dest);
std::istream& operator>>(std::istream& in, DataStruct& dest);
std::ostream& operator<<(std::ostream& out, const DataStruct& dest);

#endif

