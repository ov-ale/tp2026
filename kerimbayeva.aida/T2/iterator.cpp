#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <string>
#include <sstream>
#include <cctype>

struct DataStruct {
    unsigned long long key1;
    std::string key2;
    std::string key3;
};

std::string trim(const std::string& s) {
    size_t start = 0;
    while (start < s.length() && (s[start] == ' ' || s[start] == '\t')) {
        start++;
    }
    size_t end = s.length();
    while (end > start && (s[end - 1] == ' ' || s[end - 1] == '\t')) {
        end--;
    }
    return s.substr(start, end - start);
}

bool checkULLLit(const std::string& s) {
    std::string str = trim(s);
    if (str.length() < 3) return false;
    std::string suffix = str.substr(str.length() - 3);
    if (suffix == "ull" || suffix == "ULL") {
        for (size_t i = 0; i < str.length() - 3; i++) {
            if (!std::isdigit(str[i])) return false;
        }
        return true;
    }
    return false;
}

bool checkULLBin(const std::string& s) {
    std::string str = trim(s);
    if (str.length() < 3) return false;
    if (str[0] != '0') return false;
    if (str[1] != 'b' && str[1] != 'B') return false;
    for (size_t i = 2; i < str.length(); i++) {
        if (str[i] != '0' && str[i] != '1') return false;
    }
    return true;
}

unsigned long long binStrToULL(const std::string& s) {
    unsigned long long val = 0;
    for (size_t i = 2; i < s.length(); i++) {
        val = (val << 1) | (s[i] - '0');
    }
    return val;
}

std::istream& operator>>(std::istream& in, DataStruct& dest) {
    std::string line;
    if (!std::getline(in, line)) {
        return in;
    }
    line = trim(line);
    if (line.empty() || line.front() != '(' || line.back() != ')') {
        in.setstate(std::ios::failbit);
        return in;
    }
    line = line.substr(1, line.length() - 2);
    dest.key1 = 0;
    dest.key2 = "";
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
        while (start < line.size() && line[start] != ':'
            && line[start] != ')') {
            token += line[start];
            start++;
        }
        token = trim(token);
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
        while (start < line.size() && line[start] != ':'
            && line[start] != ')') {
            token += line[start];
            start++;
        }
        token = trim(token);
        if (checkULLBin(token)) {
            dest.key2 = token;
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
    out << "(:key1 " << src.key1 << "ull";
    out << ":key2 " << src.key2;
    out << ":key3 \"" << src.key3 << "\":)";
    return out;
}

bool compare(const DataStruct& a, const DataStruct& b) {
    if (a.key1 != b.key1) return a.key1 < b.key1;
    unsigned long long valA = binStrToULL(a.key2);
    unsigned long long valB = binStrToULL(b.key2);
    if (valA != valB) return valA < valB;
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
