// Variant 6: DBL LIT, SLL LIT

// (:key1 50.0d:key2 -89ll:key3 "some text":)
// (:key3 "world":key1 1.0d:key2 0ll:)
// (:key1 -12.34d:key2 -999ll:key3 "hello":)
// (  :key1   1.5d  :key2  10ll :key3   "ffffff" :)
// :key1 1.0d:key2 2ll:key3 "abcd":)

#include <algorithm>
#include <cctype>
#include <iomanip>
#include <ios>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>

struct DataStruct {
    double    key1;
    long long key2;
    std::string key3;
};

struct DelimiterIO {
    char exp;
};

struct DoubleLiteralIO {
    double& ref;
};

struct SLLLiteralIO {
    long long& ref;
};

struct StringIO {
    std::string& ref;
};

class iofmtguard {
public:
    iofmtguard(std::basic_ios<char>& s) :
        s_(s),
        width_(s.width()),
        fill_(s.fill()),
        precision_(s.precision()),
        fmt_(s.flags())
    {}

    ~iofmtguard() {
        s_.width(width_);
        s_.fill(fill_);
        s_.precision(precision_);
        s_.flags(fmt_);
    }

private:
    std::basic_ios<char>& s_;
    std::streamsize width_;
    char fill_;
    std::streamsize precision_;
    std::basic_ios<char>::fmtflags fmt_;
};

std::istream& operator>>(std::istream& in, DelimiterIO&& dest);
std::istream& operator>>(std::istream& in, DoubleLiteralIO&& dest);
std::istream& operator>>(std::istream& in, SLLLiteralIO&& dest);
std::istream& operator>>(std::istream& in, StringIO&& dest);
std::istream& operator>>(std::istream& in, DataStruct& dest);
std::ostream& operator<<(std::ostream& out, const DataStruct& src);

std::istream& operator>>(std::istream& in, DelimiterIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) return in;

    char c = '\0';
    in >> c;
    if (in && c != dest.exp) {
        in.setstate(std::ios::failbit);
    }
    return in;
}

std::istream& operator>>(std::istream& in, DoubleLiteralIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) return in;

    double val = 0.0;
    in >> val;
    if (!in) return in;

    char suffix = '\0';
    in.get(suffix);
    if (!in || (suffix != 'd' && suffix != 'D')) {
        in.setstate(std::ios::failbit);
        return in;
    }

    dest.ref = val;
    return in;
}

std::istream& operator>>(std::istream& in, SLLLiteralIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) return in;

    long long val = 0;
    in >> val;
    if (!in) return in;

    char l1 = '\0';
    char l2 = '\0';
    in.get(l1);
    in.get(l2);
    if (!in || !((l1 == 'l' && l2 == 'l') || (l1 == 'L' && l2 == 'L'))) {
        in.setstate(std::ios::failbit);
        return in;
    }

    dest.ref = val;
    return in;
}

std::istream& operator>>(std::istream& in, StringIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) return in;

    in >> DelimiterIO{'"'};
    if (!in) return in;

    char c = '\0';
    while (in.get(c)) {
        if (c == '\\') {
            char next = '\0';
            if (in.get(next)) {
                if (next == '"') {
                    dest.ref.push_back('"');
                } else {
                    dest.ref.push_back('\\');
                    dest.ref.push_back(next);
                }
            }
        } else if (c == '"') {
            return in;
        } else {
            dest.ref.push_back(c);
        }
    }

    in.setstate(std::ios::failbit);
    return in;
}

std::istream& operator>>(std::istream& in, DataStruct& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) return in;

    DataStruct obj{};
    bool has_key1 = false;
    bool has_key2 = false;
    bool has_key3 = false;

    in >> DelimiterIO{'('} >> DelimiterIO{':'};
    if (!in) return in;

    while (in && in.peek() != ')') {
        std::string key;
        in >> key;
        if (!in) return in;

        if (key == "key1" && !has_key1) {
            in >> DoubleLiteralIO{obj.key1};
            has_key1 = true;
        } else if (key == "key2" && !has_key2) {
            in >> SLLLiteralIO{obj.key2};
            has_key2 = true;
        } else if (key == "key3" && !has_key3) {
            in >> StringIO{obj.key3};
            has_key3 = true;
        } else {
            in.setstate(std::ios::failbit);
            return in;
        }

        if (!in) return in;

        in >> DelimiterIO{':'};
    }

    in >> DelimiterIO{')'};

    if (in && has_key1 && has_key2 && has_key3) {
        dest = std::move(obj);
    } else {
        in.setstate(std::ios::failbit);
    }

    return in;
}

std::ostream& operator<<(std::ostream& out, const DataStruct& src) {
    std::ostream::sentry sentry(out);
    if (!sentry) return out;

    iofmtguard guard(out);

    out << "(:key1 "
        << std::fixed << std::setprecision(1) << src.key1 << "d"
        << ":key2 "
        << src.key2 << "ll"
        << ":key3 \""
        << src.key3
        << "\":)";

    return out;
}

bool compareDataStruct(const DataStruct& first, const DataStruct& second) {
    const double EPSILON = 1e-9;
    if (std::abs(first.key1 - second.key1) > EPSILON) {
        return first.key1 < second.key1;
    }

    if (first.key2 != second.key2) {
        return first.key2 < second.key2;
    }

    return first.key3.length() < second.key3.length();
}

int main() {
    std::vector<DataStruct> data;

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;

        std::istringstream iss(line);
        std::copy(
            std::istream_iterator<DataStruct>(iss),
            std::istream_iterator<DataStruct>(),
            std::back_inserter(data)
        );
    }

    std::sort(data.begin(), data.end(), compareDataStruct);

    std::copy(
        data.begin(),
        data.end(),
        std::ostream_iterator<DataStruct>(std::cout, "\n")
    );

    return 0;
}
