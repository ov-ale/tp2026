#include <iostream>
#include <vector>
#include <string>
#include <iterator>
#include <algorithm>
#include <cctype>
#include <sstream>

struct DataStruct {
    unsigned long long key1{0};
    unsigned long long key2{0};
    std::string key3{""};
    size_t key2_length{0};
};

struct DelimiterIO {
    char exp;
};

std::istream& operator>>(std::istream& in, DelimiterIO&& dest) {
    std::istream::sentry sentry(in, true);
    if (!sentry) {
        return in;
    }
    char c = '\0';
    in.get(c);
    if (in && c != dest.exp) {
        in.setstate(std::ios::failbit);
    }
    return in;
}

struct UllLitIO {
    unsigned long long& ref;
};

std::istream& operator>>(std::istream& in, UllLitIO&& dest) {
    std::istream::sentry sentry(in, true);
    if (!sentry) {
        return in;
    }
    unsigned long long val = 0;
    bool hasDigits = false;
    char c = '\0';
    const unsigned long long BASE = 10;
    while (in.get(c) && std::isdigit(static_cast<unsigned char>(c))) {
        hasDigits = true;
        val = val * BASE + static_cast<unsigned long long>(c - '0');
    }
    if (!hasDigits) {
        in.setstate(std::ios::failbit);
        return in;
    }
    if (c != 'u' && c != 'U') {
        in.setstate(std::ios::failbit);
        return in;
    }
    char s2 = '\0';
    char s3 = '\0';
    in.get(s2);
    in.get(s3);
    if ((s2 == 'l' || s2 == 'L') && (s3 == 'l' || s3 == 'L')) {
        dest.ref = val;
    } else {
        in.setstate(std::ios::failbit);
    }
    return in;
}

struct UllBinIO {
    unsigned long long& ref;
    size_t& len;
};

std::istream& operator>>(std::istream& in, UllBinIO&& dest) {
    std::istream::sentry sentry(in, true);
    if (!sentry) {
        return in;
    }
    char c1 = '\0';
    char c2 = '\0';
    in.get(c1);
    in.get(c2);
    if (c1 != '0' || (c2 != 'b' && c2 != 'B')) {
        in.setstate(std::ios::failbit);
        return in;
    }
    unsigned long long val = 0;
    size_t length = 0;
    char c = '\0';
    while (in.get(c)) {
        if (c == '0' || c == '1') {
            val = (val << 1) | static_cast<unsigned long long>(c - '0');
            length++;
        } else {
            in.putback(c);
            break;
        }
    }
    if (length > 0) {
        dest.ref = val;
        dest.len = length;
        if (in.eof()) {
            in.clear(in.rdstate() & ~std::ios::failbit);
        }
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
    if (!sentry) {
        return in;
    }
    char c = '\0';
    in.get(c);
    if (c != '"') {
        in.setstate(std::ios::failbit);
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
    DataStruct temp;
    bool isKey1Found = false;
    bool isKey2Found = false;
    bool isKey3Found = false;
    const int KEYS_COUNT = 3;
    in >> DelimiterIO{'('} >> DelimiterIO{':'};
    if (!in) {
        return in;
    }
    for (int i = 0; i < KEYS_COUNT; ++i) {
        std::string key;
        char c = '\0';
        while (in.get(c) && c != ' ') {
            key.push_back(c);
        }
        if (!in) {
            return in;
        }
        if (key == "key1" && !isKey1Found) {
            in >> UllLitIO{temp.key1};
            isKey1Found = true;
        } else if (key == "key2" && !isKey2Found) {
            in >> UllBinIO{temp.key2, temp.key2_length};
            isKey2Found = true;
        } else if (key == "key3" && !isKey3Found) {
            in >> StringIO{temp.key3};
            isKey3Found = true;
        } else {
            in.setstate(std::ios::failbit);
            return in;
        }
        in >> DelimiterIO{':'};
        if (!in) {
            return in;
        }
    }
    in >> DelimiterIO{')'};
    if (in && isKey1Found && isKey2Found && isKey3Found) {
        dest = temp;
    } else {
        in.setstate(std::ios::failbit);
    }
    return in;
}

std::ostream& operator<<(std::ostream& out, const DataStruct& src) {
    std::ostream::sentry sentry(out);
    if (!sentry) {
        return out;
    }
    out << "(:key1 " << src.key1 << "ull:key2 0b";
    if (src.key2_length == 0) {
        out << src.key2;
    } else {
        std::string bin;
        unsigned long long k2 = src.key2;
        const unsigned long long BASE = 2;
        for (size_t i = 0; i < src.key2_length; ++i) {
            bin = static_cast<char>('0' + (k2 % BASE)) + bin;
            k2 /= BASE;
        }
        out << bin;
    }
    out << ":key3 \"" << src.key3 << "\":)";
    return out;
}

bool compareDataStruct(const DataStruct& a, const DataStruct& b) {
    if (a.key1 != b.key1) {
        return a.key1 < b.key1;
    }
    if (a.key2 != b.key2) {
        return a.key2 < b.key2;
    }
    return a.key3.length() < b.key3.length();
}

int main() {
    std::vector<DataStruct> data;
    std::string line;
    while (std::getline(std::cin, line)) {
        std::istringstream lineStream(line);
        std::copy(
            std::istream_iterator<DataStruct>(lineStream),
            std::istream_iterator<DataStruct>(),
            std::back_inserter(data)
        );
    }
    std::sort(data.begin(), data.end(), compareDataStruct);
    std::copy(
        data.begin(),
        data.end(),
        std::ostream_iterator<DataStruct>(std::cout, "\n")
    );
    return 0;
}
