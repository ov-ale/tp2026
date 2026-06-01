#include "DataStruct.h"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>

int main() {
  std::vector<DataStruct> data;
  std::copy(std::istream_iterator<DataStruct>(std::cin),
  std::istream_iterator<DataStruct>(),
  std::back_inserter(data));

  if (data.empty()) {
    std::cout << "Looks like there is no supported record. Cannot determine input. Test skipped\n";
  } else {
    std::cout << "Atleast one supported record type\n";
    std::sort(data.begin(), data.end(), compareDataStructs);
    std::copy(data.begin(), data.end(),
    std::ostream_iterator<DataStruct>(std::cout, "\n"));
  }
  return 0;
}
