#include <iostream>
#include <algorithm>
#include <vector>
#include <iterator>
#include <complex>
#include <string>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <limits>

struct DataStruct {
    unsigned long long key1;
    std::complex<double> key2;
    std::string key3;

    bool operator<(const DataStruct& other) const;
};

std::istream& operator>>(std::istream& in, DataStruct& dest);
std::ostream& operator<<(std::ostream& out, const DataStruct& dest);

bool DataStruct::operator<(const DataStruct& other) const {
    if (key1 != other.key1) {
        return key1 < other.key1;
    }

    double diff = std::abs(key2) - std::abs(other.key2);
    if (std::abs(diff) > std::numeric_limits<double>::epsilon()) {
        return std::abs(key2) < std::abs(other.key2);
    }

    return key3.length() < other.key3.length();
}

struct Delimiter {
    char exp;
};

struct OctDataIO {
    unsigned long long& ref;
};

struct ComplexDataIO {
    std::complex<double>& ref;
};

struct StringDataIO {
    std::string& ref;
};

std::istream& operator>>(std::istream& in, Delimiter&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) {
        return in;
    }

    char c;
    in >> c;

    if (in && c != dest.exp) {
        in.setstate(std::ios::failbit);
    }
    return in;
}

std::istream& operator>>(std::istream& in, OctDataIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) {
        return in;
    }

    if (in.peek() != '0') {
        in.setstate(std::ios::failbit);
        return in;
    }

    if (!(in >> std::oct >> dest.ref)) {
        return in;
    }

    in >> std::dec;
    return in;
}


std::istream& operator>>(std::istream& in, ComplexDataIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) {
        return in;
    }

    double real = 0.0;
    double imag = 0.0;

    if (!(in >> Delimiter{'#'} >> Delimiter{'c'} >> Delimiter{'('} >> real >> imag >> Delimiter{')'})) {
        return in;
    }

    dest.ref = {real, imag};
    return in;
}

std::istream& operator>>(std::istream& in, StringDataIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) {
        return in;
    }

    if (!(in >> Delimiter{'"'})) {
        return in;
    }

    std::getline(in, dest.ref, '"');
    return in;
}

std::istream& operator>>(std::istream& in, DataStruct& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) {
        return in;
    }

    if (!(in >> Delimiter{'('} >> Delimiter{':'})) {
        return in;
    }

    for (size_t i = 0; i < 3; ++i) {
        std::string key;

        if (!(in >> key)) {
            return in;
        }

        if (key == "key1") {
            in >> OctDataIO{dest.key1};
        } else if (key == "key2") {
            in >> ComplexDataIO{dest.key2};
        } else if (key == "key3") {
            in >> StringDataIO{dest.key3};
        } else {
            in.setstate(std::ios::failbit);
            return in;
        }
        in >> Delimiter{':'};
    }

    in >> Delimiter{')'};
    return in;
}

std::ostream& operator<<(std::ostream& out, const DataStruct& dest) {
    std::ostream::sentry sentry(out);
    if (!sentry) {
        return out;
    }

    out << "(:key1 0" << std::oct << dest.key1 << std::dec;

    out << ":key2 #c(" << std::fixed << std::setprecision(1) << dest.key2.real() << " " << dest.key2.imag() << ")";

    out << ":key3 \"" << dest.key3 << "\":)";

    return out;
}

int main() {
    std::vector<DataStruct> data;
    std::string line;

    while (std::getline(std::cin, line)) {
        if (line.empty()) {
            continue;
        }
        std::istringstream iss(line);

        std::copy(
        std::istream_iterator<DataStruct>(iss),
        std::istream_iterator<DataStruct>(),
        std::back_inserter(data)
        );
    }

    std::sort(data.begin(), data.end());

    std::copy(
        data.begin(),
        data.end(),
        std::ostream_iterator<DataStruct>(std::cout, "\n")
    );
    return 0;
}
