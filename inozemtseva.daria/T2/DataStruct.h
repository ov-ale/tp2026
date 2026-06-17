#ifndef DATA_STRUCT_H
#define DATA_STRUCT_H
#include <complex>
#include <iostream>
#include <string>
#include <iomanip>

struct DataStruct {
  unsigned long long key1_;
  std::complex<double> key2_;
  std::string key3_;
};

std::istream& operator>>(std::istream& in, DataStruct& d);
std::ostream& operator<<(std::ostream& out, const DataStruct& d);
bool compareDataStructs(const DataStruct& a, const DataStruct& b);

#endif
