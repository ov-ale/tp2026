#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <string>
#include <sstream>
#include <cctype>
#include <limits>

struct DataStruct {
    unsigned long long key1;
    unsigned long long key2;
    std::string key3;
};

struct DelimiterIO {
    char exp;
};

struct ULLLitIO {
    unsigned long long& ref;
};

struct ULLBinIO {
    unsigned long long& ref;
};

struct StringIO {
    std::string& ref;
};

std::istream& operator>>(std::istream& in, DelimiterIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) return in;
    char c;
    in >> c;
    if (in && c != dest.exp) {
        in.setstate(std::ios::failbit);
    }
    return in;
}

std::istream& operator>>(std::istream& in, ULLLitIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) return in;

    std::string num;
    in >> num;
    if (!in) return in;

    if (num.size() < 3) {
        in.setstate(std::ios::failbit);
        return in;
    }

    std::string suffix = num.substr(num.size() - 3);
    if (suffix != "ull" && suffix != "ULL") {
        in.setstate(std::ios::failbit);
        return in;
    }

    std::string val = num.substr(0, num.size() - 3);
    for (char c : val) {
        if (!std::isdigit(c)) {
            in.setstate(std::ios::failbit);
            return in;
        }
    }

    dest.ref = std::stoull(val);
    return in;
}

std::istream& operator>>(std::istream& in, ULLBinIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) return in;

    std::string bin;
    in >> bin;
    if (!in) return in;

    if (bin.size() >= 3 && bin[0] == '0' && (bin[1] == 'b' || bin[1] == 'B')) {
        dest.ref = std::stoull(bin.substr(2), nullptr, 2);
    }
    else {
        in.setstate(std::ios::failbit);
    }
    return in;
}

std::istream& operator>>(std::istream& in, StringIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) return in;
    return std::getline(in >> DelimiterIO{ '"' }, dest.ref, '"');
}

std::istream& operator>>(std::istream& in, DataStruct& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) return in;

    char ch;
    while (in >> ch && ch != '(') {}
    if (!in) return in;

    DataStruct tmp;
    bool k1 = false, k2 = false, k3 = false;
    std::string token;

    while (in >> token) {
        if (token == ":key1" && !k1) {
            in >> ULLLitIO{ tmp.key1 };
            k1 = true;
        }
        else if (token == ":key2" && !k2) {
            in >> ULLBinIO{ tmp.key2 };
            k2 = true;
        }
        else if (token == ":key3" && !k3) {
            in >> StringIO{ tmp.key3 };
            k3 = true;
        }
        else if (token == ":)") {
            break;
        }
        else {
            in.clear();
            in.ignore(std::numeric_limits<std::streamsize>::max(), ')');
            in.setstate(std::ios::failbit);
            return in;
        }
    }

    if (k1 && k2 && k3) {
        dest = tmp;
    }
    else {
        in.setstate(std::ios::failbit);
    }
    return in;
}

std::ostream& operator<<(std::ostream& out, const DataStruct& src) {
    out << "(:key1 " << src.key1 << "ull";
    out << ":key2 0b";
    if (src.key2 == 0) {
        out << "0";
    }
    else {
        std::string bin;
        unsigned long long n = src.key2;
        while (n > 0) {
            bin = (n & 1 ? '1' : '0') + bin;
            n >>= 1;
        }
        out << bin;
    }
    out << ":key3 \"" << src.key3 << "\":)";
    return out;
}

bool compare(const DataStruct& a, const DataStruct& b) {
    if (a.key1 != b.key1) return a.key1 < b.key1;
    if (a.key2 != b.key2) return a.key2 < b.key2;
    return a.key3.length() < b.key3.length();
}

int main() {
    std::vector<DataStruct> data;

    std::copy(
        std::istream_iterator<DataStruct>(std::cin),
        std::istream_iterator<DataStruct>(),
        std::back_inserter(data)
    );

    std::sort(data.begin(), data.end(), compare);

    std::copy(
        data.begin(),
        data.end(),
        std::ostream_iterator<DataStruct>(std::cout, "\n")
    );

    return 0;
}
