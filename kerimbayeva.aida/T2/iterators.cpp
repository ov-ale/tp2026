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

    dest.key1 = 0;
    dest.key2 = 0;
    dest.key3 = "";

    while (!line.empty() && std::isspace(line.front())) {
        line.erase(0, 1);
    }
    while (!line.empty() && std::isspace(line.back())) {
        line.pop_back();
    }

    if (line.empty() || line.front() != '(' || line.back() != ')') {
        in.setstate(std::ios::failbit);
        return in;
    }

    line = line.substr(1, line.size() - 2);

    for (char& c : line) {
        if (c == ':') c = ' ';
    }

    std::istringstream iss(line);
    std::string name;
    std::string value;

    bool key1Ok = false;
    bool key2Ok = false;
    bool key3Ok = false;

    while (iss >> name >> value) {
        if (name == "key1") {
            if (value.size() >= 3) {
                std::string suffix = value.substr(value.size() - 3);
                if (suffix == "ull" || suffix == "ULL") {
                    dest.key1 = std::stoull(
                        value.substr(0, value.size() - 3));
                    key1Ok = true;
                }
            }
        }
        else if (name == "key2") {
            if (value.size() >= 3 && value[0] == '0' &&
                (value[1] == 'b' || value[1] == 'B')) {
                dest.key2 = std::stoull(value.substr(2), nullptr, 2);
                key2Ok = true;
            }
        }
        else if (name == "key3") {
            if (value.size() >= 2 && value.front() == '"' &&
                value.back() == '"') {
                dest.key3 = value.substr(1, value.size() - 2);
                key3Ok = true;
            }
        }
    }

    if (!key1Ok || !key2Ok || !key3Ok) {
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
    std::istream_iterator<DataStruct> it(std::cin);
    std::istream_iterator<DataStruct> end;
    std::copy(it, end, std::back_inserter(data));
    std::sort(data.begin(), data.end(), compare);
    std::copy(data.begin(), data.end(),
        std::ostream_iterator<DataStruct>(std::cout, "\n"));
    return 0;
}
