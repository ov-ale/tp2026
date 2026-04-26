#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <string>
#include <sstream>

struct DataStruct {
    unsigned long long key1;
    unsigned long long key2;
    std::string key3;
};

std::istream& operator>>(std::istream& in, DataStruct& d) {
    std::string line;
    if (!std::getline(in, line)) return in;

    std::istringstream iss(line);
    char c;
    iss >> c;
    if (c != '(') {
        in.setstate(std::ios::failbit);
        return in;
    }

    std::string token;
    bool k1 = false, k2 = false, k3 = false;

    while (iss >> token) {
        if (token == ":key1") {
            std::string v;
            iss >> v;
            if (v.size() > 3) {
                d.key1 = std::stoull(v.substr(0, v.size() - 3));
                k1 = true;
            }
        }
        else if (token == ":key2") {
            std::string v;
            iss >> v;
            if (v.size() > 2 && v[0] == '0' && v[1] == 'b') {
                d.key2 = std::stoull(v.substr(2), nullptr, 2);
                k2 = true;
            }
        }
        else if (token == ":key3") {
            std::string v;
            iss >> v;
            if (v.size() > 1 && v[0] == '"') {
                d.key3 = v.substr(1, v.size() - 2);
                k3 = true;
            }
        }
        else if (token == ":)") break;
    }

    if (!k1 || !k2 || !k3) in.setstate(std::ios::failbit);
    return in;
}

std::ostream& operator<<(std::ostream& out, const DataStruct& d) {
    out << "(:key1 " << d.key1 << "ull";
    out << ":key2 0b";

    if (d.key2 == 0) {
        out << "0";
    }
    else {
        std::string bin;
        unsigned long long n = d.key2;
        while (n) {
            bin = char('0' + (n & 1)) + bin;
            n >>= 1;
        }
        out << bin;
    }

    out << ":key3 \"" << d.key3 << "\":)";
    return out;
}

bool cmp(const DataStruct& a, const DataStruct& b) {
    if (a.key1 != b.key1) return a.key1 < b.key1;
    if (a.key2 != b.key2) return a.key2 < b.key2;
    return a.key3.size() < b.key3.size();
}

int main() {
    std::vector<DataStruct> v;
    std::copy(std::istream_iterator<DataStruct>(std::cin),
        std::istream_iterator<DataStruct>(),
        std::back_inserter(v));
    std::sort(v.begin(), v.end(), cmp);
    std::copy(v.begin(), v.end(),
        std::ostream_iterator<DataStruct>(std::cout, "\n"));
    return 0;
}
