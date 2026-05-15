#include <algorithm>
#include <cctype>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

struct DataStruct {
    unsigned long long key1{};
    unsigned long long key2{};
    std::string key3;
};

struct BinaryRecord {
    DataStruct data;
    std::string text;
};

std::vector<BinaryRecord> records;

bool sameData(const DataStruct& a, const DataStruct& b) {
    return a.key1 == b.key1 && a.key2 == b.key2 && a.key3 == b.key3;
}

void rememberBinary(const DataStruct& data, const std::string& text) {
    records.push_back({ data, text });
}

std::string takeBinary(const DataStruct& data) {
    for (size_t i = 0; i < records.size(); ++i) {
        if (sameData(records[i].data, data)) {
            std::string result = records[i].text;
            records.erase(records.begin() + static_cast<long>(i));
            return result;
        }
    }
    return "";
}

void printBinary(std::ostream& out, unsigned long long val) {
    out << "0b";
    if (val == 0) {
        out << "0";
        return;
    }
    std::string s;
    unsigned long long temp = val;
    while (temp > 0) {
        s += char('0' + (temp % 2));
        temp /= 2;
    }
    std::reverse(s.begin(), s.end());
    out << s;
}

struct Delim { char exp; };

std::istream& operator>>(std::istream& in, Delim&& dest) {
    std::istream::sentry sentry(in, true);
    if (!sentry) return in;
    char c = '\0';
    in.get(c);
    if (!in || c != dest.exp) {
        in.setstate(std::ios::failbit);
    }
    return in;
}

struct KeyIO { std::string& ref; };

std::istream& operator>>(std::istream& in, KeyIO&& dest) {
    std::istream::sentry sentry(in, true);
    if (!sentry) return in;
    dest.ref.clear();
    while (in) {
        int next = in.peek();
        if (next == std::char_traits<char>::eof() || next == ' ') break;
        dest.ref += static_cast<char>(in.get());
    }
    if (dest.ref.empty()) {
        in.setstate(std::ios::failbit);
    }
    return in;
}

struct ULLiteralIO { unsigned long long& ref; };

std::istream& operator>>(std::istream& in, ULLiteralIO&& dest) {
    std::istream::sentry sentry(in, true);
    if (!sentry) return in;
    std::string token;
    while (in) {
        int next = in.peek();
        if (next == std::char_traits<char>::eof() || next == ' ') break;
        token += static_cast<char>(in.get());
    }
    if (token.size() < 4) {
        in.setstate(std::ios::failbit);
        return in;
    }
    std::string suffix = token.substr(token.size() - 3);
    if (suffix != "ull" && suffix != "ULL") {
        in.setstate(std::ios::failbit);
        return in;
    }
    std::string numberPart = token.substr(0, token.size() - 3);
    if (numberPart.empty()) {
        in.setstate(std::ios::failbit);
        return in;
    }
    for (char c : numberPart) {
        if (!std::isdigit(static_cast<unsigned char>(c))) {
            in.setstate(std::ios::failbit);
            return in;
        }
    }
    try {
        dest.ref = std::stoull(numberPart);
    }
    catch (...) {
        in.setstate(std::ios::failbit);
    }
    return in;
}

struct BinUllIO { unsigned long long& ref; std::string& raw; };

std::istream& operator>>(std::istream& in, BinUllIO&& dest) {
    std::istream::sentry sentry(in, true);
    if (!sentry) return in;
    std::string token;
    while (in) {
        int next = in.peek();
        if (next == std::char_traits<char>::eof() || next == ' ') break;
        token += static_cast<char>(in.get());
    }
    if (token.size() < 3) {
        in.setstate(std::ios::failbit);
        return in;
    }
    if (token[0] != '0' || (token[1] != 'b' && token[1] != 'B')) {
        in.setstate(std::ios::failbit);
        return in;
    }
    std::string numberPart = token.substr(2);
    if (numberPart.empty()) {
        in.setstate(std::ios::failbit);
        return in;
    }
    for (char c : numberPart) {
        if (c != '0' && c != '1') {
            in.setstate(std::ios::failbit);
            return in;
        }
    }
    try {
        dest.ref = std::stoull(numberPart, nullptr, 2);
        dest.raw = numberPart;
    }
    catch (...) {
        in.setstate(std::ios::failbit);
    }
    return in;
}

struct StringIO { std::string& ref; };

std::istream& operator>>(std::istream& in, StringIO&& dest) {
    std::istream::sentry sentry(in, true);
    if (!sentry) return in;
    char c = '\0';
    in.get(c);
    if (!in || c != '"') {
        in.setstate(std::ios::failbit);
        return in;
    }
    dest.ref.clear();
    while (in.get(c) && c != '"') {
        dest.ref += c;
    }
    if (!in) {
        in.setstate(std::ios::failbit);
    }
    return in;
}

std::istream& operator>>(std::istream& in, DataStruct& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) return in;

    while (in && std::isspace(in.peek())) in.get();

    if (in.peek() != '(') {
        in.setstate(std::ios::failbit);
        return in;
    }
    in.get();

    DataStruct temp;
    bool hasKey1 = false, hasKey2 = false, hasKey3 = false;
    std::string key2Raw;

    for (int i = 0; i < 3; ++i) {
        std::string key;
        in >> KeyIO{ key };
        if (!in) return in;
        if (!(in >> Delim{ ' ' })) return in;

        if (key == "key1" && !hasKey1) {
            in >> ULLiteralIO{ temp.key1 };
            hasKey1 = true;
        }
        else if (key == "key2" && !hasKey2) {
            in >> BinUllIO{ temp.key2, key2Raw };
            hasKey2 = true;
        }
        else if (key == "key3" && !hasKey3) {
            in >> StringIO{ temp.key3 };
            hasKey3 = true;
        }
        else {
            in.setstate(std::ios::failbit);
            return in;
        }

        if (!in) return in;
        if (!(in >> Delim{ ' ' })) return in;
    }

    if (in.peek() != ')') {
        in.setstate(std::ios::failbit);
        return in;
    }
    in.get();

    if (hasKey1 && hasKey2 && hasKey3) {
        dest = temp;
        rememberBinary(dest, key2Raw);
    }
    else {
        in.setstate(std::ios::failbit);
    }
    return in;
}

std::ostream& operator<<(std::ostream& out, const DataStruct& src) {
    out << "(key1 " << src.key1 << "ull";
    out << " key2 ";
    std::string raw = takeBinary(src);
    if (!raw.empty()) {
        out << "0b" << raw;
    }
    else {
        printBinary(out, src.key2);
    }
    out << " key3 \"" << src.key3 << "\")";
    return out;
}

bool compareDataStruct(const DataStruct& a, const DataStruct& b) {
    if (a.key1 != b.key1) return a.key1 < b.key1;
    if (a.key2 != b.key2) return a.key2 < b.key2;
    return a.key3 < b.key3;
}

int main() {
    std::vector<DataStruct> data;
    std::string line;
    bool hasAny = false;

    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;
        std::istringstream iss(line);
        DataStruct ds;
        if (iss >> ds) {
            data.push_back(ds);
            hasAny = true;
        }
    }

    if (!hasAny) {
        std::cout << "Looks like there is no supported record. Cannot determine input. Test skipped" << std::endl;
        return 0;
    }

    std::sort(data.begin(), data.end(), compareDataStruct);

    for (const auto& item : data) {
        std::cout << item << "\n";
    }

    return 0;
}
