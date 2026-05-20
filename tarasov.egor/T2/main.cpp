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

struct LabelIO {
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
std::istream& operator>>(std::istream& in, LabelIO&& dest);
std::istream& operator>>(std::istream& in, DataStruct& dest);
std::ostream& operator<<(std::ostream& out, const DataStruct& src);

std::istream& operator>>(std::istream& in, DelimiterIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) return in;

    char c = '0';
    in >> c;
    if (in && c != dest.exp) {
        in.setstate(std::ios::failbit);
    }
    return in;
}

std::istream& operator>>(std::istream& in, DoubleLiteralIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) return in;

    bool negative = false;
    if (in.peek() == '-') {
        negative = true;
        in.get();
    }
    else if (in.peek() == '+') {
        in.get();
    }

    std::string integer_part;
    while (std::isdigit(static_cast<unsigned char>(in.peek()))) {
        integer_part.push_back(static_cast<char>(in.get()));
    }
    if (integer_part.empty()) {
        in.setstate(std::ios::failbit);
        return in;
    }

    if (in.peek() != '.') {
        in.setstate(std::ios::failbit);
        return in;
    }
    in.get();

    std::string frac_part;
    while (std::isdigit(static_cast<unsigned char>(in.peek()))) {
        frac_part.push_back(static_cast<char>(in.get()));
    }
    if (frac_part.empty()) {
        in.setstate(std::ios::failbit);
        return in;
    }

    char suffix = static_cast<char>(in.peek());
    if (suffix != 'd' && suffix != 'D') {
        in.setstate(std::ios::failbit);
        return in;
    }
    in.get();

    try {
        std::string raw = (negative ? "-" : "") + integer_part + "." + frac_part;
        dest.ref = std::stod(raw);
    }
    catch (...) {
        in.setstate(std::ios::failbit);
    }
    return in;
}

std::istream& operator>>(std::istream& in, SLLLiteralIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) return in;

    bool negative = false;
    if (in.peek() == '-') {
        negative = true;
        in.get();
    }
    else if (in.peek() == '+') {
        in.get();
    }

    std::string digits;
    while (std::isdigit(static_cast<unsigned char>(in.peek()))) {
        digits.push_back(static_cast<char>(in.get()));
    }
    if (digits.empty()) {
        in.setstate(std::ios::failbit);
        return in;
    }

    char l1 = static_cast<char>(in.get());
    char l2 = static_cast<char>(in.get());
    if (!in || !((l1 == 'l' && l2 == 'l') || (l1 == 'L' && l2 == 'L'))) {
        in.setstate(std::ios::failbit);
        return in;
    }

    try {
        std::string raw = (negative ? "-" : "") + digits;
        dest.ref = std::stoll(raw);
    }
    catch (...) {
        in.setstate(std::ios::failbit);
    }

    return in;
}

std::istream& operator>>(std::istream& in, StringIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) return in;
    return std::getline(in >> DelimiterIO{'"'}, dest.ref, '"');
}

std::istream& operator>>(std::istream& in, LabelIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) return in;

    dest.ref.clear();
    char c = '0';
    while (in.get(c) && !std::isspace(static_cast<unsigned char>(c))) {
        dest.ref.push_back(c);
    }

    if (!in || in.peek() == ' ' || dest.ref.empty()) {
        in.setstate(std::ios::failbit);
    }

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
        std::string field_name;
        in >> LabelIO{field_name};
        if (!in) return in;

        if (field_name == "key1" && !has_key1) {
            in >> DoubleLiteralIO{obj.key1};
            has_key1 = true;
        }
        else if (field_name == "key2" && !has_key2) {
            in >> SLLLiteralIO{obj.key2};
            has_key2 = true;
        }
        else if (field_name == "key3" && !has_key3) {
            in >> StringIO{obj.key3};
            has_key3 = true;
        }
        else {
            in.setstate(std::ios::failbit);
            return in;
        }
        
        if (!in) return in;

        in >> DelimiterIO{':'};
    }

    in >> DelimiterIO{')'};

    if (in && has_key1 && has_key2 && has_key3) {
        dest = std::move(obj);
    }
    else {
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
    if (first.key1 != second.key1) {
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
