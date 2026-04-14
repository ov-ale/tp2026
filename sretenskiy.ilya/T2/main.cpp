#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <cctype>

const double EPSILON = 1e-9;

struct DataStruct {
    double key1;
    char key2;
    std::string key3;
};

static std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\n\r\f\v");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\n\r\f\v");
    return s.substr(start, end - start + 1);
}

bool parseString(const std::string& str, std::string& value) {
    if (str.size() >= 2 && str.front() == '"' && str.back() == '"') {
        value = str.substr(1, str.size() - 2);
        return true;
    }
    return false;
}

bool parseDoubleSci(const std::string& str, double& value) {
    std::istringstream iss(str);
    char sign = '+';
    if (iss.peek() == '-') {
        iss.get(sign);
    }

    std::string mantissaStr;
    char c;
    bool hasDecimalPoint = false;
    bool hasDigitBeforePoint = false;
    bool hasDigitAfterPoint = false;

    while (iss.get(c)) {
        if (std::isdigit(static_cast<unsigned char>(c))) {
            mantissaStr += c;
            if (!hasDecimalPoint)
                hasDigitBeforePoint = true;
            else
                hasDigitAfterPoint = true;
        }
        else if (c == '.' && !hasDecimalPoint) {
            if (!hasDigitBeforePoint) return false;
            mantissaStr += c;
            hasDecimalPoint = true;
        }
        else if (c == 'e' || c == 'E') {
            break;
        }
        else {
            return false;
        }
    }

    if (!hasDecimalPoint || !hasDigitAfterPoint) return false;

    char exponentSign = '+';
    if (!(iss.get(exponentSign)) || (exponentSign != '+' && exponentSign != '-')) return false;

    int exponent = 0;
    if (!(iss >> exponent)) return false;

    char leftover;
    if (iss >> leftover) return false;

    double mantissa = std::stod(mantissaStr);
    if (sign == '-') mantissa = -mantissa;
    value = mantissa * std::pow(10.0, (exponentSign == '-') ? -exponent : exponent);
    return true;
}

bool parseCharLit(const std::string& str, char& value) {
    if (str.size() >= 3 && str.front() == '\'' && str.back() == '\'') {
        value = str[1];
        return true;
    }
    return false;
}

bool parseRecord(const std::string& line, DataStruct& data) {
    std::string t = trim(line);
    if (t.size() < 4) return false;
    if (t[0] != '(' || t[1] != ':') return false;
    if (t[t.size() - 2] != ':' || t.back() != ')') return false;

    std::string content = t.substr(1, t.size() - 2);
    double k1 = 0.0;
    char k2 = '\0';
    std::string k3;
    bool found1 = false, found2 = false, found3 = false;

    size_t pos1 = content.find(":key1 ");
    if (pos1 != std::string::npos) {
        size_t start_val = pos1 + 6;
        size_t end_val = content.find(':', start_val);
        if (end_val != std::string::npos) {
            std::string val = trim(content.substr(start_val, end_val - start_val));
            if (parseDoubleSci(val, k1)) found1 = true;
        }
    }

    size_t pos2 = content.find(":key2 ");
    if (pos2 != std::string::npos) {
        size_t start_val = pos2 + 6;
        size_t end_val = content.find(':', start_val);
        if (end_val != std::string::npos) {
            std::string val = trim(content.substr(start_val, end_val - start_val));
            if (parseCharLit(val, k2)) found2 = true;
        }
    }

    size_t pos3 = content.find(":key3 ");
    if (pos3 != std::string::npos) {
        size_t start_val = pos3 + 6;
        size_t end_val = content.find(':', start_val);
        if (end_val != std::string::npos) {
            std::string val = trim(content.substr(start_val, end_val - start_val));
            if (parseString(val, k3)) found3 = true;
        }
    }

    if (found1 && found2 && found3) {
        data.key1 = k1;
        data.key2 = k2;
        data.key3 = k3;
        return true;
    }
    return false;
}

std::istream& operator>>(std::istream& in, DataStruct& data) {
    std::string line;
    while (std::getline(in, line)) {
        DataStruct tmp;
        if (parseRecord(line, tmp)) {
            data = tmp;
            return in;
        }
    }
    in.setstate(std::ios::failbit);
    return in;
}

static void printDoubleSci(std::ostream& out, double value) {
    if (value == 0.0) {
        out << "0.0e+0";
        return;
    }
    double mantissa = value;
    int exponent = 0;
    if (std::abs(mantissa) >= 10.0) {
        while (std::abs(mantissa) >= 10.0) {
            mantissa /= 10.0;
            exponent++;
        }
    }
    else if (std::abs(mantissa) < 1.0 && mantissa != 0.0) {
        while (std::abs(mantissa) < 1.0) {
            mantissa *= 10.0;
            exponent--;
        }
    }
    out << std::fixed << std::setprecision(1) << mantissa
        << "e" << (exponent >= 0 ? "+" : "") << exponent;
}

std::ostream& operator<<(std::ostream& out, const DataStruct& data) {
    out << "(:key1 ";
    printDoubleSci(out, data.key1);
    out << ":key2 '" << data.key2 << "':key3 \"" << data.key3 << "\":)";
    return out;
}

bool compare(const DataStruct& a, const DataStruct& b) {
    if (std::abs(a.key1 - b.key1) > EPSILON) {
        return a.key1 < b.key1;
    }
    if (a.key2 != b.key2) {
        return a.key2 < b.key2;
    }
    return a.key3.size() < b.key3.size();
}

int main() {
    std::vector<DataStruct> data;
    std::copy(std::istream_iterator<DataStruct>(std::cin), std::istream_iterator<DataStruct>(), std::back_inserter(data));
    std::sort(data.begin(), data.end(), compare);
    std::copy(data.begin(), data.end(),
        std::ostream_iterator<DataStruct>(std::cout, "\n"));
    return 0;
}