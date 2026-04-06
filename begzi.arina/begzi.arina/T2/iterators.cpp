#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <iterator>
#include <algorithm>
#include <iomanip>
#include <limits>
#include <cctype>

struct DataStruct {
    double key1 = 0;
    unsigned long long key2 = 0;
    std::string key3;
};

struct DelimiterIO { char exp; };
struct KeyIO { std::string& ref; };
struct DoubleSciIO { double& ref; };
struct BinUllIO { unsigned long long& ref; };
struct StringIO { std::string& ref; };

struct iofmtguard {
    iofmtguard(std::ostream& s) : s_(s), fill_(s.fill()), precision_(s.precision()), fmt_(s.flags()) {}
    ~iofmtguard() { s_.fill(fill_); s_.precision(precision_); s_.flags(fmt_); }
private:
    std::ostream& s_;
    char fill_;
    std::streamsize precision_;
    std::ios::fmtflags fmt_;
};

void printBinary(std::ostream& out, unsigned long long val) {
    out << "0b";
    if (val == 0) {
        out << "0";
        return;
    }
    std::string s;
    unsigned long long tmp = val;
    while (tmp) {
        s += (tmp & 1) ? '1' : '0';
        tmp >>= 1;
    }
    std::reverse(s.begin(), s.end());
    out << s;
}

std::istream& operator>>(std::istream& in, DelimiterIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) return in;
    char c;
    in >> c;
    if (in && std::tolower(c) != std::tolower(dest.exp))
        in.setstate(std::ios::failbit);
    return in;
}

std::istream& operator>>(std::istream& in, KeyIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) return in;
    dest.ref.clear();
    char c;
    while (in >> c && std::isalnum(static_cast<unsigned char>(c))) {
        dest.ref += c;
        if (!std::isalnum(static_cast<unsigned char>(in.peek()))) break;
    }
    return in;
}

std::istream& operator>>(std::istream& in, DoubleSciIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) return in;
    in >> std::ws;
    std::string temp;
    std::getline(in, temp, ':');
    in.putback(':');
    bool has_e = (temp.find('e') != std::string::npos || temp.find('E') != std::string::npos);
    bool has_dot = (temp.find('.') != std::string::npos);
    if (!has_e || !has_dot) {
        in.setstate(std::ios::failbit);
        return in;
    }
    try {
        dest.ref = std::stod(temp);
    }
    catch (...) {
        in.setstate(std::ios::failbit);
    }
    return in;
}

std::istream& operator>>(std::istream& in, BinUllIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) return in;
    in >> std::ws;
    char c1, c2;
    if (!(in >> c1 >> c2) || !(c1 == '0' && std::tolower(c2) == 'b')) {
        in.setstate(std::ios::failbit);
        return in;
    }
    std::string bin;
    while (true) {
        char ch = in.peek();
        if (ch == '0' || ch == '1') {
            in.get(ch);
            bin += ch;
        }
        else break;
    }
    if (bin.empty()) {
        in.setstate(std::ios::failbit);
        return in;
    }
    try {
        dest.ref = std::stoull(bin, nullptr, 2);
    }
    catch (...) {
        in.setstate(std::ios::failbit);
    }
    return in;
}

std::istream& operator>>(std::istream& in, StringIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) return in;
    char c;
    if (!(in >> c) || c != '"') {
        in.setstate(std::ios::failbit);
        return in;
    }
    std::getline(in, dest.ref, '"');
    return in;
}

std::istream& operator>>(std::istream& in, DataStruct& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) return in;

    char ch;
    while (in.get(ch)) {
        if (ch == '(') break;
    }
    if (!in) return in;

    if (!(in >> DelimiterIO{ ':' })) {
        in.clear();
        in.ignore(std::numeric_limits<std::streamsize>::max(), ')');
        return in;
    }

    DataStruct tmp;
    int fields = 0;
    bool ok = true;

    for (int i = 0; i < 3 && ok; ++i) {
        std::string key;
        in >> KeyIO{ key };
        if (!in) { ok = false; break; }

        if (key == "key1") in >> DoubleSciIO{ tmp.key1 };
        else if (key == "key2") in >> BinUllIO{ tmp.key2 };
        else if (key == "key3") in >> StringIO{ tmp.key3 };
        else { ok = false; break; }

        if (!in) { ok = false; break; }
        ++fields;

        if (i < 2) {
            if (!(in >> DelimiterIO{ ':' })) { ok = false; break; }
        }
    }

    if (ok && fields == 3 && (in >> DelimiterIO{ ':' }) && (in >> DelimiterIO{ ')' })) {
        dest = tmp;
        return in;
    }

    in.clear();
    in.ignore(std::numeric_limits<std::streamsize>::max(), ')');
    return in;
}

std::ostream& operator<<(std::ostream& out, const DataStruct& src) {
    iofmtguard guard(out);
    std::ostringstream oss;
    oss << std::scientific << std::setprecision(1) << std::nouppercase << src.key1;
    std::string num = oss.str();
    size_t epos = num.find('e');
    if (epos != std::string::npos && epos + 3 <= num.size() && num[epos + 2] == '0') {
        num.erase(epos + 2, 1);
    }
    out << "(:key1 " << num << ":key2 ";
    printBinary(out, src.key2);
    out << ":key3 \"" << src.key3 << "\":)";
    return out;
}

bool compareData(const DataStruct& a, const DataStruct& b) {
    if (a.key1 != b.key1) return a.key1 < b.key1;
    if (a.key2 != b.key2) return a.key2 < b.key2;
    return a.key3.size() < b.key3.size();
}

int main() {
    std::vector<DataStruct> storage;
    std::copy(
        std::istream_iterator<DataStruct>(std::cin),
        std::istream_iterator<DataStruct>(),
        std::back_inserter(storage)
    );
    std::sort(storage.begin(), storage.end(), compareData);
    std::copy(
        storage.begin(),
        storage.end(),
        std::ostream_iterator<DataStruct>(std::cout, "\n")
    );
    return 0;
}
