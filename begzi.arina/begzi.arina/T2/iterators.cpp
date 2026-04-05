#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include <iomanip>
#include <cctype>
#include <sstream>

struct DataStruct {
    double key1;
    unsigned long long key2;
    std::string key3;
};

struct DelimiterIO { char expected; };
struct DoubleScientificIO { double& value; };
struct UnsignedBinaryIO { unsigned long long& value; };
struct StringIO { std::string& value; };

std::istream& operator>>(std::istream& in, DelimiterIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) return in;
    char c;
    in >> std::ws >> c;
    if (in && std::tolower(static_cast<unsigned char>(c)) != std::tolower(static_cast<unsigned char>(dest.expected))) {
        in.setstate(std::ios::failbit);
    }
    return in;
}

std::istream& operator>>(std::istream& in, DoubleScientificIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) return in;
    std::string token;
    if (!(in >> token)) return in;

    size_t ePos = token.find_first_of("eE");
    if (ePos == std::string::npos || ePos == 0 || ePos == token.size() - 1) {
        in.setstate(std::ios::failbit);
        return in;
    }
    std::string mantissa = token.substr(0, ePos);
    std::string exponent = token.substr(ePos + 1);

    size_t dotPos = mantissa.find('.');
    if (dotPos == std::string::npos || dotPos == 0 || dotPos == mantissa.size() - 1) {
        in.setstate(std::ios::failbit);
        return in;
    }
    for (size_t i = 0; i < mantissa.size(); ++i) {
        if (i != dotPos && !std::isdigit(static_cast<unsigned char>(mantissa[i]))) {
            in.setstate(std::ios::failbit);
            return in;
        }
    }

    size_t expStart = 0;
    if (!exponent.empty() && (exponent[0] == '+' || exponent[0] == '-')) expStart = 1;
    if (expStart >= exponent.size()) {
        in.setstate(std::ios::failbit);
        return in;
    }
    for (size_t i = expStart; i < exponent.size(); ++i) {
        if (!std::isdigit(static_cast<unsigned char>(exponent[i]))) {
            in.setstate(std::ios::failbit);
            return in;
        }
    }
    try {
        dest.value = std::stod(token);
    }
    catch (...) {
        in.setstate(std::ios::failbit);
    }
    return in;
}

std::istream& operator>>(std::istream& in, UnsignedBinaryIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) return in;
    char c1, c2;
    if (!(in >> c1 >> c2) || c1 != '0' || std::tolower(static_cast<unsigned char>(c2)) != 'b') {
        in.setstate(std::ios::failbit);
        return in;
    }
    std::string bin;
    char ch;
    while (in.get(ch)) {
        if (ch == '0' || ch == '1') {
            bin += ch;
        }
        else {
            in.putback(ch);
            break;
        }
    }
    if (bin.empty()) {
        in.setstate(std::ios::failbit);
        return in;
    }
    try {
        dest.value = std::stoull(bin, nullptr, 2);
    }
    catch (...) {
        in.setstate(std::ios::failbit);
    }
    return in;
}

std::istream& operator>>(std::istream& in, StringIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) return in;
    char quote;
    if (!(in >> quote) || quote != '"') {
        in.setstate(std::ios::failbit);
        return in;
    }
    std::getline(in, dest.value, '"');
    return in;
}

std::istream& operator>>(std::istream& in, DataStruct& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) return in;
    DataStruct input;
    if (!(in >> DelimiterIO{ '(' } >> DelimiterIO{ ':' })) return in;

    bool hasKey1 = false, hasKey2 = false, hasKey3 = false;
    while (true) {
        std::string label;
        if (!(in >> label)) {
            in.setstate(std::ios::failbit);
            break;
        }
        if (label == "key1") {
            if (hasKey1) { in.setstate(std::ios::failbit); break; }
            in >> DoubleScientificIO{ input.key1 };
            hasKey1 = true;
        }
        else if (label == "key2") {
            if (hasKey2) { in.setstate(std::ios::failbit); break; }
            in >> UnsignedBinaryIO{ input.key2 };
            hasKey2 = true;
        }
        else if (label == "key3") {
            if (hasKey3) { in.setstate(std::ios::failbit); break; }
            in >> StringIO{ input.key3 };
            hasKey3 = true;
        }
        else {
            in.setstate(std::ios::failbit);
            break;
        }
        if (in.fail()) break;

        in >> std::ws;
        char next = in.peek();
        if (next == ':') {
            in.get();
            continue;
        }
        else if (next == ')') {
            in.get();
            break;
        }
        else {
            in.setstate(std::ios::failbit);
            break;
        }
    }

    if (hasKey1 && hasKey2 && hasKey3) {
        dest = input;
    }
    else {
        in.setstate(std::ios::failbit);
    }
    return in;
}

std::string formatBinary(unsigned long long value) {
    if (value == 0) return "0";
    std::string result;
    while (value > 0) {
        result += (value & 1) ? '1' : '0';
        value >>= 1;
    }
    std::reverse(result.begin(), result.end());
    return result;
}

struct iofmtguard {
    iofmtguard(std::ostream& s) : s_(s), fill_(s.fill()), precision_(s.precision()), fmt_(s.flags()) {}
    ~iofmtguard() { s_.fill(fill_); s_.precision(precision_); s_.flags(fmt_); }
private:
    std::ostream& s_;
    char fill_;
    std::streamsize precision_;
    std::ios::fmtflags fmt_;
};

std::ostream& operator<<(std::ostream& out, const DataStruct& src) {
    iofmtguard guard(out);
    out << "(:key1 " << std::scientific << std::setprecision(1) << src.key1;
    out << ":key2 0b" << formatBinary(src.key2);
    out << ":key3 \"" << src.key3 << "\":)";
    return out;
}

bool compareData(const DataStruct& a, const DataStruct& b) {
    if (a.key1 < b.key1) return true;
    if (b.key1 < a.key1) return false;
    if (a.key2 < b.key2) return true;
    if (b.key2 < a.key2) return false;
    return a.key3.length() < b.key3.length();
}

int main() {
    std::vector<DataStruct> storage;
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;
        std::istringstream iss(line);
        DataStruct temp;
        if (iss >> temp) {
            storage.push_back(std::move(temp));
        }
    }
    std::sort(storage.begin(), storage.end(), compareData);
    std::copy(storage.begin(), storage.end(),
        std::ostream_iterator<DataStruct>(std::cout, "\n"));
    return 0;
}
