#include <iostream>
#include <vector>
#include <string>
#include <complex>
#include <algorithm>
#include <iterator>
#include <iomanip>
#include <limits>

namespace nspace {
  struct DataStruct {
    unsigned long long key1;
    std::complex<double> key2;
    std::string key3;
  };

  struct Delim {
    char expected;
  };

  std::istream& operator>>(std::istream& in, Delim&& d) {
    std::istream::sentry sentry(in);
    if (!sentry) return in;
    char c;
    in >> c;
    if (std::tolower(c) != std::tolower(d.expected)) {
      in.setstate(std::ios::failbit);
    }
    return in;
  }

  std::istream& operator>>(std::istream& in, DataStruct& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) return in;
    DataStruct temp{0, {0.0, 0.0}, ""};
    if (!(in >> Delim{'('} >> Delim{':'})) return in;
    for (int i = 0; i < 3; ++i) {
      std::string key;
      std::getline(in, key, ' ');
      if (key == "key1") {
        in >> std::hex >> temp.key1 >> std::dec >> Delim{':'};
      } else if (key == "key2") {
        double re, im;
        in >> Delim{'#'} >> Delim{'c'} >> Delim{'('} >> re >> im >> Delim{')'} >> Delim{':'};
        temp.key2 = {re, im};
      } else if (key == "key3") {
        in >> Delim{'"'};
        std::getline(in, temp.key3, '"');
        in >> Delim{':'};
      } else {
        in.setstate(std::ios::failbit);
      }
    }
    if (in >> Delim{')'}) dest = temp;
    return in;
  }

  std::ostream& operator<<(std::ostream& out, const DataStruct& src) {
    std::ostream::sentry sentry(out);
    if (!sentry) return out;
    out << "(:key1 0x" << std::uppercase << std::hex << src.key1 << std::dec << std::nouppercase;
    out << ":key2 #c(" << std::fixed << std::setprecision(1) << src.key2.real() << " " << src.key2.imag() << ")";
    out << ":key3 \"" << src.key3 << "\":)";
    return out;
  }

  bool compareData(const DataStruct& a, const DataStruct& b) {
    if (a.key1 != b.key1) return a.key1 < b.key1;
    if (std::abs(a.key2) != std::abs(b.key2)) return std::abs(a.key2) < std::abs(b.key2);
    return a.key3.length() < b.key3.length();
  }
}

int main() {
  using namespace nspace;
  std::vector<DataStruct> data;
  while (!std::cin.eof()) {
    std::copy(
      std::istream_iterator<DataStruct>(std::cin),
      std::istream_iterator<DataStruct>(),
      std::back_inserter(data)
    );
    if (std::cin.fail() && !std::cin.eof()) {
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
  }
  std::sort(data.begin(), data.end(), compareData);
  std::copy(
    data.begin(),
    data.end(),
    std::ostream_iterator<DataStruct>(std::cout, "\n")
  );
  return 0;
}
