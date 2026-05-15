#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <string>
#include <sstream>
#include <cctype>
#include <limits>
#include <stdexcept>

struct DataStruct {
    unsigned long long key1;
    unsigned long long key2;
    std::string key3;
};

struct DelimiterIO {
    char exp;
};

struct ULLIO {
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

std::istream& operator>>(std::istream& in, ULLIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) return in;

    std::string token;
    in >> token;
    if (!in) return in;

    std::string clean = token;

    if (clean.size() >= 3) {
        std::string suffix = clean.substr(clean.size() - 3);
        if (suffix == "ull" || suffix == "ULL") {
            clean = clean.substr(0, clean.size() - 3);
        }
    }

    if (clean.empty()) {
        in.setstate(std::ios::failbit);
        return in;
    }

    try {
        if (clean.size() >= 2 && clean[0] == '0' && (clean[1] == 'x' || clean[1] == 'X')) {
            dest.ref = std::stoull(clean, nullptr, 16);
            return in;
        }

        if (clean.size() >= 2 && clean[0] == '0' && (clean[1] == 'b' || clean[1] == 'B')) {
            dest.ref = std::stoull(clean.substr(2), nullptr, 2);
            return in;
        }

        if (clean.size() > 1 && clean[0] == '0') {
            dest.ref = std::stoull(clean, nullptr, 8);
            return in;
        }

        dest.ref = std::stoull(clean);
        return in;
    }
    catch (const std::invalid_argument&) {
        in.setstate(std::ios::failbit);
        return in;
    }
    catch (const std::out_of_range&) {
        in.setstate(std::ios::failbit);
        return in;
    }
}

std::istream& operator>>(std::istream& in, StringIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) return in;
    return std::getline(in >> DelimiterIO{ '"' }, dest.ref, '"');
}

std::istream& operator>>(std::istream& in, DataStruct& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) return in;

    char c;
    while (in >> c) {
        if (c == '(') break;
        if (in.eof()) return in;
    }
    if (!in) return in;

    DataStruct temp;
    bool k1 = false, k2 = false, k3 = false;
    bool fail = false;

    while (in && !fail) {
        while (std::isspace(in.peek())) in.get();

        if (in.peek() == ')') {
            in.get();
            break;
        }

        if (in.peek() != ':') {
            fail = true;
            break;
        }

        in.get();

        std::string key;
        in >> key;

        if (key == "key1" && !k1) {
            in >> ULLIO{ temp.key1 };
            if (in) {
                k1 = true;
            }
            else {
                fail = true;
                in.clear();
            }
        }
        else if (key == "key2" && !k2) {
            in >> ULLIO{ temp.key2 };
            if (in) {
                k2 = true;
            }
            else {
                fail = true;
                in.clear();
            }
        }
        else if (key == "key3" && !k3) {
            in >> StringIO{ temp.key3 };
            if (in) {
                k3 = true;
            }
            else {
                fail = true;
                in.clear();
            }
        }
        else {
            fail = true;
        }
    }

    if (!fail && k1 && k2 && k3) {
        dest = temp;
        return in;
    }

    in.clear();
    while (in && in.peek() != ')') {
        in.get();
    }
    if (in) in.get();
    in.setstate(std::ios::failbit);
    return in;
}

std::ostream& operator<<(std::ostream& out, const DataStruct& src) {
    out << "(:key1 " << src.key1 << "ull";
    out << ":key2 0b";

    if (src.key2 == 0) {
        out << "0";
    }
    else {
        std::string binary;
        unsigned long long n = src.key2;
        while (n > 0) {
            binary = (n & 1 ? '1' : '0') + binary;
            n >>= 1;
        }
        out << binary;
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

    if (data.empty()) {
        std::cout << "Looks like there is no supported record. Cannot determine input. Test skipped\n";
        return 0;
    }

    std::cout << "Atleast one supported record type\n";

    std::sort(data.begin(), data.end(), compare);

    std::copy(
        data.begin(),
        data.end(),
        std::ostream_iterator<DataStruct>(std::cout, "\n")
    );

    return 0;
}
