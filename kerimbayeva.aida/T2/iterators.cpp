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

bool isULLLit(const std::string& s) {
    if (s.length() < 4) return false;
    for (size_t i = 0; i < s.length() - 3; i++) {
        if (!std::isdigit(s[i])) return false;
    }
    std::string suf = s.substr(s.length() - 3);
    return (suf == "ull" || suf == "ULL");
}

bool isULLBin(const std::string& s) {
    if (s.length() < 3) return false;
    if (s[0] != '0') return false;
    if (s[1] != 'b' && s[1] != 'B') return false;
    for (size_t i = 2; i < s.length(); i++) {
        if (s[i] != '0' && s[i] != '1') return false;
    }
    return true;
}

std::istream& operator>>(std::istream& in, DataStruct& d) {
    std::string line;
    if (!std::getline(in, line)) return in;

    size_t l = line.find('(');
    size_t r = line.find(')');
    if (l == std::string::npos || r == std::string::npos) {
        in.setstate(std::ios::failbit);
        return in;
    }

    std::string s = line.substr(l + 1, r - l - 1);
    std::istringstream iss(s);

    d.key1 = 0;
    d.key2 = 0;
    d.key3 = "";

    bool k1 = false, k2 = false, k3 = false;
    std::string token;

    while (iss >> token) {
        if (token == ":key1") {
            std::string val;
            if (!(iss >> val)) break;
            if (isULLLit(val)) {
                d.key1 = std::stoull(val.substr(0, val.size() - 3));
                k1 = true;
            }
        }
        else if (token == ":key2") {
            std::string val;
            if (!(iss >> val)) break;
            if (isULLBin(val)) {
                d.key2 = std::stoull(val.substr(2), nullptr, 2);
                k2 = true;
            }
        }
        else if (token == ":key3") {
            std::string val;
            if (!(iss >> val)) break;
            if (val.size() >= 2 && val.front() == '"' && val.back() == '"') {
                d.key3 = val.substr(1, val.size() - 2);
                k3 = true;
            }
        }
    }

    if (!k1 || !k2 || !k3) {
        in.setstate(std::ios::failbit);
    }
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
        while (n > 0) {
            bin = (n & 1 ? '1' : '0') + bin;
            n >>= 1;
        }
        out << bin;
    }
    out << ":key3 \"" << d.key3 << "\":)";
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
