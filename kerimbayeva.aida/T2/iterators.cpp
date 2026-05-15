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

struct UllLitIO {
    unsigned long long& ref;
};

struct UllBinIO {
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

std::istream& operator>>(std::istream& in, UllLitIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) return in;

    std::string token;
    in >> token;
    if (!in) return in;

    token.erase(0, token.find_first_not_of(" \t"));
    token.erase(token.find_last_not_of(" \t") + 1);

    if (token.size() >= 3) {
        std::string suffix = token.substr(token.size() - 3);
        if (suffix == "ull" || suffix == "ULL") {
            std::string numStr = token.substr(0, token.size() - 3);
            bool valid = !numStr.empty();
            for (char c : numStr) {
                if (!std::isdigit(c)) {
                    valid = false;
                    break;
                }
            }
            if (valid) {
                dest.ref = std::stoull(numStr);
                return in;
            }
        }
    }

    std::istringstream iss(token);
    if (iss >> dest.ref) {
        return in;
    }

    in.setstate(std::ios::failbit);
    return in;
}

std::istream& operator>>(std::istream& in, UllBinIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) return in;

    std::string token;
    in >> token;
    if (!in) return in;

    if (token.size() >= 3 && token[0] == '0' && (token[1] == 'b' || token[1] == 'B')) {
        std::string binStr = token.substr(2);
        binStr.erase(0, binStr.find_first_not_of(" \t"));
        binStr.erase(binStr.find_last_not_of(" \t") + 1);
        dest.ref = std::stoull(binStr, nullptr, 2);
        return in;
    }

    in.setstate(std::ios::failbit);
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
            in >> UllLitIO{ temp.key1 };
            if (in) k1 = true;
            else fail = true;
        }
        else if (key == "key2" && !k2) {
            in >> UllBinIO{ temp.key2 };
            if (in) k2 = true;
            else fail = true;
        }
        else if (key == "key3" && !k3) {
            in >> StringIO{ temp.key3 };
            if (in) k3 = true;
            else fail = true;
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

    if (std::cin.peek() == EOF) {
        std::cout << "Looks like there is no supported record. Cannot determine input. Test skipped\n";
        return 0;
    }

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
