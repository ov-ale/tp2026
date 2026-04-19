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

bool checkULLLit(const std::string& s) {
    if (s.length() < 3) return false;
    std::string suffix = s.substr(s.length() - 3);
    if (suffix == "ull" || suffix == "ULL") {
        for (size_t i = 0; i < s.length() - 3; i++) {
            if (!std::isdigit(s[i])) return false;
        }
        return true;
    }
    return false;
}

bool checkULLBin(const std::string& s) {
    if (s.length() < 3) return false;
    if (s[0] != '0') return false;
    if (s[1] != 'b' && s[1] != 'B') return false;
    for (size_t i = 2; i < s.length(); i++) {
        if (s[i] != '0' && s[i] != '1') return false;
    }
    return true;
}

std::istream& operator>>(std::istream& in, DataStruct& dest) {
    std::string line;
    if (!std::getline(in, line)) {
        return in;
    }

    if (line.find('(') == std::string::npos || line.find(')') == std::string::npos) {
        in.setstate(std::ios::failbit);
        return in;
    }

    dest.key1 = 0;
    dest.key2 = 0;
    dest.key3 = "";

    bool key1Valid = false;
    bool key2Valid = false;
    bool key3Valid = false;

    size_t pos1 = line.find("key1");
    if (pos1 != std::string::npos) {
        size_t start = pos1 + 5;
        while (start < line.size() && std::isspace(line[start])) {
            start++;
        }
        std::string token;
        while (start < line.size() && line[start] != ':' && line[start] != ')') {
            token += line[start];
            start++;
        }
        while (!token.empty() && std::isspace(token.back())) {
            token.pop_back();
        }
        if (checkULLLit(token)) {
            std::string numStr;
            for (char c : token) {
                if (std::isdigit(c)) numStr += c;
                else break;
            }
            dest.key1 = std::stoull(numStr);
            key1Valid = true;
        }
    }

    size_t pos2 = line.find("key2");
    if (pos2 != std::string::npos) {
        size_t start = pos2 + 5;
        while (start < line.size() && std::isspace(line[start])) {
            start++;
        }
        std::string token;
        while (start < line.size() && line[start] != ':' && line[start] != ')') {
            token += line[start];
            start++;
        }
        while (!token.empty() && std::isspace(token.back())) {
            token.pop_back();
        }
        if (checkULLBin(token)) {
            std::string binStr = token.substr(2);
            dest.key2 = std::stoull(binStr, nullptr, 2);
            key2Valid = true;
        }
    }

    size_t pos3 = line.find("key3");
    if (pos3 != std::string::npos) {
        size_t start = line.find('"', pos3);
        if (start != std::string::npos) {
            size_t end = line.find('"', start + 1);
            if (end != std::string::npos) {
                dest.key3 = line.substr(start + 1, end - start - 1);
                key3Valid = true;
            }
        }
    }

    if (!key1Valid || !key2Valid || !key3Valid) {
        in.setstate(std::ios::failbit);
    }

    return in;
}

std::ostream& operator<<(std::ostream& out, const DataStruct& src) {
    out << "(:key1 " << src.key1 << "u11";
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
    std::string line;

    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;
        std::istringstream iss(line);
        DataStruct ds;
        if (iss >> ds) {
            data.push_back(ds);
        }
    }

    std::sort(data.begin(), data.end(), compare);

    std::copy(data.begin(), data.end(),
        std::ostream_iterator<DataStruct>(std::cout, "\n"));

    return 0;
}
