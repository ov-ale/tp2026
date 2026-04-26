#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <string>
#include <sstream>
#include <cctype>

struct DataStruct {
    unsigned long long key1;
    unsigned long long key2;
    std::string key3;
};

std::istream& operator>>(std::istream& in, DataStruct& dest) {
    std::string line;
    if (!std::getline(in, line)) {
        return in;
    }

    std::istringstream iss(line);

    char ch;
    iss >> ch;
    if (ch != '(') {
        in.setstate(std::ios::failbit);
        return in;
    }

    std::string field;
    while (iss >> field) {
        if (field == ":key1") {
            std::string val;
            iss >> val;
            size_t len = val.length();
            if (len >= 3 && (val.substr(len - 3) == "ull" ||
                val.substr(len - 3) == "ULL")) {
                dest.key1 = std::stoull(val.substr(0, len - 3));
            }
            else {
                in.setstate(std::ios::failbit);
                return in;
            }
        }
        else if (field == ":key2") {
            std::string val;
            iss >> val;
            if (val.length() >= 3 && val[0] == '0' &&
                (val[1] == 'b' || val[1] == 'B')) {
                dest.key2 = std::stoull(val.substr(2), nullptr, 2);
            }
            else {
                in.setstate(std::ios::failbit);
                return in;
            }
        }
        else if (field == ":key3") {
            std::string val;
            iss >> val;
            if (val.length() >= 2 && val.front() == '"' &&
                val.back() == '"') {
                dest.key3 = val.substr(1, val.length() - 2);
            }
            else {
                in.setstate(std::ios::failbit);
                return in;
            }
        }
        else if (field == ":)") {
            break;
        }
        else {
            in.setstate(std::ios::failbit);
            return in;
        }
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
    std::istream_iterator<DataStruct> it(std::cin);
    std::istream_iterator<DataStruct> end;
    std::copy(it, end, std::back_inserter(data));
    std::sort(data.begin(), data.end(), compare);
    std::copy(data.begin(), data.end(),
        std::ostream_iterator<DataStruct>(std::cout, "\n"));
    return 0;
}
