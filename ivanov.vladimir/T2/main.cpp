#include <iostream>
#include <vector>
#include <string>
#include <iterator>
#include <algorithm>
#include <limits>
#include <cctype>
#include <sstream>
#include <utility>

struct DataStruct {
    unsigned long long key1;
    unsigned long long key2;
    std::string key3;
    std::string key2raw;

    DataStruct() : key1(0), key2(0), key3(""), key2raw("") {}
    DataStruct(const DataStruct&) = default;
    DataStruct(DataStruct&&) = default;
    DataStruct& operator=(const DataStruct&) = default;
    DataStruct& operator=(DataStruct&&) = default;
    ~DataStruct() = default;
};

struct DelimiterIO {
    char exp;
};

std::istream& operator>>(std::istream& in, DelimiterIO&& dest) {
    std::istream::sentry sentry(in, true);
    if (!sentry) return in;
    char c = '\0';
    in.get(c);
    if (!in || c != dest.exp) {
        in.setstate(std::ios::failbit);
    }
    return in;
}

struct UllLitIO {
    unsigned long long& ref;
};

std::istream& operator>>(std::istream& in, UllLitIO&& dest) {
    std::istream::sentry sentry(in, true);
    if (!sentry) return in;

    std::string digits;
    int c = in.peek();
    while (in && c != std::char_traits<char>::eof() &&
           std::isdigit(static_cast<unsigned char>(c))) {
        digits.push_back(static_cast<char>(in.get()));
        c = in.peek();
    }

    if (digits.empty() || (digits.size() > 1 && digits.front() == '0')) {
        in.setstate(std::ios::failbit);
        return in;
    }

    unsigned long long value = 0;
    for (char ch : digits) {
        const unsigned digit = static_cast<unsigned>(ch - '0');
        if (value > (std::numeric_limits<unsigned long long>::max() - digit) / 10) {
            in.setstate(std::ios::failbit);
            return in;
        }
        value = value * 10 + digit;
    }

    char s1 = '\0';
    char s2 = '\0';
    char s3 = '\0';
    in.get(s1);
    in.get(s2);
    in.get(s3);

    if (in &&
        std::tolower(static_cast<unsigned char>(s1)) == 'u' &&
        std::tolower(static_cast<unsigned char>(s2)) == 'l' &&
        std::tolower(static_cast<unsigned char>(s3)) == 'l') {
        dest.ref = value;
    } else {
        in.setstate(std::ios::failbit);
    }

    return in;
}

struct UllBinIO {
    unsigned long long& ref;
    std::string& raw;
};

std::istream& operator>>(std::istream& in, UllBinIO&& dest) {
    std::istream::sentry sentry(in, true);
    if (!sentry) return in;

    dest.raw.clear();

    char p1 = '\0';
    char p2 = '\0';
    in.get(p1);
    in.get(p2);

    if (!(in && p1 == '0' && std::tolower(static_cast<unsigned char>(p2)) == 'b')) {
        in.setstate(std::ios::failbit);
        return in;
    }

    unsigned long long value = 0;
    bool hasDigits = false;
    int c = in.peek();

    while (in && c != std::char_traits<char>::eof() && (c == '0' || c == '1')) {
        hasDigits = true;
        char ch = static_cast<char>(in.get());
        dest.raw.push_back(ch);
        const unsigned digit = static_cast<unsigned>(ch - '0');
        if (value > (std::numeric_limits<unsigned long long>::max() - digit) / 2) {
            in.setstate(std::ios::failbit);
            return in;
        }
        value = value * 2 + digit;
        c = in.peek();
    }

    if (hasDigits) {
        dest.ref = value;
    } else {
        in.setstate(std::ios::failbit);
    }

    return in;
}

struct StringIO {
    std::string& ref;
};

std::istream& operator>>(std::istream& in, StringIO&& dest) {
    std::istream::sentry sentry(in, true);
    if (!sentry) return in;

    char c = '\0';
    in.get(c);
    if (in && c == '"' && std::getline(in, dest.ref, '"')) {
        return in;
    }

    in.setstate(std::ios::failbit);
    return in;
}

bool readKey(std::istream& in, std::string& key) {
    key.clear();
    int c = in.peek();

    while (in &&
           c != std::char_traits<char>::eof() &&
           c != ' ' &&
           c != ':' &&
           c != '(' &&
           c != ')' &&
           !std::isspace(static_cast<unsigned char>(c))) {
        key.push_back(static_cast<char>(in.get()));
        c = in.peek();
    }

    if (key.empty()) {
        in.setstate(std::ios::failbit);
        return false;
    }

    return true;
}

std::istream& operator>>(std::istream& in, DataStruct& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) return in;

    DataStruct temp;
    bool k1 = false;
    bool k2 = false;
    bool k3 = false;

    in >> DelimiterIO{'('} >> DelimiterIO{':'};

    for (int i = 0; i < 3 && in; ++i) {
        std::string key;
        if (!readKey(in, key)) {
            return in;
        }

        in >> DelimiterIO{' '};
        if (!in) {
            return in;
        }

        if (key == "key1") {
            if (k1) {
                in.setstate(std::ios::failbit);
                return in;
            }
            in >> UllLitIO{temp.key1};
            k1 = true;
        } else if (key == "key2") {
            if (k2) {
                in.setstate(std::ios::failbit);
                return in;
            }
            in >> UllBinIO{temp.key2, temp.key2raw};
            k2 = true;
        } else if (key == "key3") {
            if (k3) {
                in.setstate(std::ios::failbit);
                return in;
            }
            in >> StringIO{temp.key3};
            k3 = true;
        } else {
            in.setstate(std::ios::failbit);
            return in;
        }

        in >> DelimiterIO{':'};
    }

    in >> DelimiterIO{')'};

    if (in && k1 && k2 && k3) {
        in >> std::ws;
        if (in.eof()) {
            dest = std::move(temp);
        } else {
            in.setstate(std::ios::failbit);
        }
    }

    return in;
}

std::ostream& operator<<(std::ostream& out, const DataStruct& src) {
    std::ostream::sentry sentry(out);
    if (!sentry) return out;

    out << "(:key1 " << src.key1 << "ull:key2 0b";

    if (!src.key2raw.empty()) {
        out << src.key2raw;
    } else if (src.key2 == 0) {
        out << '0';
    } else {
        std::string bin;
        unsigned long long k2 = src.key2;
        while (k2 > 0) {
            bin = char('0' + (k2 % 2)) + bin;
            k2 /= 2;
        }
        out << bin;
    }

    out << ":key3 \"" << src.key3 << "\":)";
    return out;
}

bool compareDS(const DataStruct& a, const DataStruct& b) {
    if (a.key1 != b.key1) return a.key1 < b.key1;
    if (a.key2 != b.key2) return a.key2 < b.key2;
    return a.key3.length() < b.key3.length();
}

int main() {
    std::vector<DataStruct> data;
    std::string line;

    while (std::getline(std::cin, line)) {
        std::istringstream lineStream(line);
        std::copy(std::istream_iterator<DataStruct>(lineStream),
                  std::istream_iterator<DataStruct>(),
                  std::back_inserter(data));
    }

    std::sort(data.begin(), data.end(), compareDS);
    std::copy(data.begin(), data.end(), std::ostream_iterator<DataStruct>(std::cout, "\n"));

    return 0;
}
