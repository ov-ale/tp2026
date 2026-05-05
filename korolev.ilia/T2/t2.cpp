#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include <limits>
#include <utility>
#include <cmath>

struct DataStruct {
    char key1;
    std::pair<long long, unsigned long long> key2;
    std::string key3;
};

struct DelimiterIO {
    char exp;
};
struct RationalIO {
    std::pair<long long, unsigned long long>& ref;
};
struct StringIO {
    std::string& ref;
};

std::istream& operator>>(std::istream& in, DelimiterIO dest) {
    std::istream::sentry sentry(in);
    if (!sentry) return in;
    char c;
    in >> c;
    if (in && c != dest.exp) {
        in.setstate(std::ios::failbit);
    }
    return in;
}

std::istream& operator>>(std::istream& in, RationalIO dest) {
    std::istream::sentry sentry(in);
    if (!sentry) return in;
    long long n;
    unsigned long long d;
    in >> DelimiterIO{'('} >> DelimiterIO{':'} >> DelimiterIO{'N'} >> n;
    in >> DelimiterIO{':'} >> DelimiterIO{'D'} >> d;
    in >> DelimiterIO{':'} >> DelimiterIO{')'};
    if (in && d != 0) {
        dest.ref = {n, d};
    } else {
        in.setstate(std::ios::failbit);
    }
    return in;
}

std::istream& operator>>(std::istream& in, StringIO dest) {
    std::istream::sentry sentry(in);
    if (!sentry) return in;
    return std::getline(in >> DelimiterIO{'"'}, dest.ref, '"');
}

std::istream& operator>>(std::istream& in, DataStruct& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) return in;

    DataStruct input;
    in >> DelimiterIO{'('} >> DelimiterIO{':'};

    for (int i = 0; i < 3; ++i) {
        std::string key;
        char c;
        while (in >> c && std::isallnum(c)) {
            key += c;
        }
        if (in) {
            in.putback(c);
        }

        if (key == "key1") {
            in >> DelimiterIO{'\''} >> input.key1 >> DelimiterIO{'\''};
        } else if (key == "key2") {
            in >> RationalIO{input.key2};
        } else if (key == "key3") {
            in >> StringIO{input.key3};
        } else {
            in.setstate(std::ios::failbit);
        }

        if (i < 2) {
            in >> DelimiterIO{':'};
        }
    }

    in >> DelimiterIO{':'} >> DelimiterIO{')'};

    if (in) {
        dest = input;
    }
    return in;
}

std::ostream& operator<<(std::ostream& os, const DataStruct& obj) {
    std::ostream::sentry sentry(os);
    if (!sentry) return os;
    os << "(:key1 '" << obj.key1 << "':key2 (:N " << obj.key2.first
       << ":D " << obj.key2.second << ":):key3 \"" << obj.key3 << "\":)";
    return os;
}

struct Comparator {
    bool operator()(const DataStruct& a, const DataStruct& b) const {
        if (a.key1 != b.key1) return a.key1 < b.key1;

        double va = static_cast<double>(a.key2.first) / a.key2.second;
        double vb = static_cast<double>(b.key2.first) / b.key2.second;
        if (std::abs(va - vb) >= 1e-9) return va < vb;

        return a.key3.length() < b.key3.length();
    }
};

int main() {
    std::vector<DataStruct> data;

    while (true) {
        DataStruct tmp;
        if (std::cin >> tmp) {
            data.push_back(tmp);
        } else {
            if (std::cin.eof()) {
                break;
            }
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    std::sort(data.begin(), data.end(), Comparator());

    std::copy(
        data.begin(),
        data.end(),
        std::ostream_iterator<DataStruct>(std::cout, "\n")
    );

    return 0;
}
