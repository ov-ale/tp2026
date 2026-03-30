#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <iomanip>
#include <complex>
#include <cmath>
#include <cctype>

struct DelimiterIO{
    char exp;
};

struct LabelIO{
    std::string exp;
};

struct ULLBinIO{
    unsigned long long& ref;
};

struct ComplexIO{
    std::complex<double>& ref;
};

struct StringIO{
    std::string& ref;
};

struct DataStruct{
    unsigned long long key1;
    std::complex<double> key2;
    std::string key3;
};

class iofmtguard{
public:
    iofmtguard(std::basic_ios<char>& s);
    ~iofmtguard();
private:
    std::basic_ios<char>& s_;
    std::streamsize width_;
    char fill_;
    std::streamsize precision_;
    std::basic_ios<char>::fmtflags fmt_;
};

std::istream& operator>>(std::istream& in, DelimiterIO&& dest);
std::istream& operator>>(std::istream& in, LabelIO&& dest);
std::istream& operator>>(std::istream& in, ULLBinIO&& dest);
std::istream& operator>>(std::istream& in, ComplexIO&& dest);
std::istream& operator>>(std::istream& in, StringIO&& dest);
std::istream& operator>>(std::istream& in, DataStruct& dest);
std::ostream& operator<<(std::ostream& out, const DataStruct& dest);

bool compareDataStruct(const DataStruct& a, const DataStruct& b){
    if (a.key1 != b.key1) {
        return a.key1 < b.key1;
    }
    double mod_a = std::abs(a.key2);
    double mod_b = std::abs(b.key2);
    if (mod_a != mod_b) {
        return mod_a < mod_b;
    }
    return a.key3.length() < b.key3.length();
}

int main(){
    std::vector<DataStruct> data;

    std::copy(
        std::istream_iterator<DataStruct>(std::cin),
        std::istream_iterator<DataStruct>(),
        std::back_inserter(data)
    );

    std::sort(data.begin(), data.end(), compareDataStruct);

    std::copy(
        data.begin(),
        data.end(),
        std::ostream_iterator<DataStruct>(std::cout, "\n")
    );

    return 0;
}

iofmtguard::iofmtguard(std::basic_ios<char>& s) :
    s_(s),
    width_(s.width()),
    fill_(s.fill()),
    precision_(s.precision()),
    fmt_(s.flags())
{}

iofmtguard::~iofmtguard(){
    s_.width(width_);
    s_.fill(fill_);
    s_.precision(precision_);
    s_.flags(fmt_);
}

std::istream& operator>>(std::istream& in, DelimiterIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) {
        return in;
    }
    char c = '0';
    in >> c;
    if (in && (c != dest.exp)) {
        in.setstate(std::ios::failbit);
    }
    return in;
}

std::istream& operator>>(std::istream& in, StringIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) {
        return in;
    }
    return std::getline(in >> DelimiterIO{ '"' }, dest.ref, '"');
}

std::istream& operator>>(std::istream& in, LabelIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) {
        return in;
    }

    std::string data = "";
    if (!(in >> StringIO{ data })) {
        return in;
    }

    if (data != dest.exp) {
        in.setstate(std::ios::failbit);
    }
    return in;
}

std::istream& operator>>(std::istream& in, ULLBinIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) {
        return in;
    }

    char c = '0';
    in >> c;

    if (c != '0') {
        in.setstate(std::ios::failbit);
        return in;
    }

    in >> c;
    if (c != 'b' && c != 'B') {
        in.setstate(std::ios::failbit);
        return in;
    }

    std::string binary_str;
    while (in.get(c) && (c == '0' || c == '1')) {
        binary_str += c;
    }
    in.unget();

    if (binary_str.empty()) {
        in.setstate(std::ios::failbit);
        return in;
    }

    dest.ref = 0;
    for (char bit : binary_str) {
        dest.ref = (dest.ref << 1) | (bit - '0');
    }

    return in;
}

std::istream& operator>>(std::istream& in, ComplexIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) {
        return in;
    }
    double real = 0.0, imag = 0.0;
    in >> DelimiterIO{ '#' } >> DelimiterIO{ 'c' } >> DelimiterIO{ '(' };
    in >> real >> imag;
    in >> DelimiterIO{ ')' };
    if (in) {
        dest.ref = std::complex<double>(real, imag);
    }

    return in;
}

std::istream& operator>>(std::istream& in, DataStruct& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) {
        return in;
    }
    DataStruct input;
    in >> DelimiterIO{ '(' };
    bool key1_read = false;
    bool key2_read = false;
    bool key3_read = false;
    while (in && (!key1_read || !key2_read || !key3_read)) {
        std::string label;

        if (!(in >> StringIO{ label })) {
            in.setstate(std::ios::failbit);
            break;
        }

        in >> DelimiterIO{ ':' };

        if (label == "key1" && !key1_read) {
            in >> ULLBinIO{ input.key1 };
            key1_read = true;
        }
        else if (label == "key2" && !key2_read) {
            in >> ComplexIO{ input.key2 };
            key2_read = true;
        }
        else if (label == "key3" && !key3_read) {
            in >> StringIO{ input.key3 };
            key3_read = true;
        }
        else {
            in.setstate(std::ios::failbit);
            break;
        }

        char c;
        while (in.get(c) && std::isspace(c)) {}
        in.unget();

        if (in.peek() != ':' && in.peek() != ')') {
            in >> DelimiterIO{ ':' };
        }
    }

    in >> DelimiterIO{ ')' };

    if (in && key1_read && key2_read && key3_read) {
        dest = input;
    }
    else {
        in.setstate(std::ios::failbit);
    }

    return in;
}

std::ostream& operator<<(std::ostream& out, const DataStruct& src) {
    std::ostream::sentry sentry(out);
    if (!sentry) {
        return out;
    }

    iofmtguard fmtguard(out);

    out << "(:key1 0b";

    if (src.key1 == 0) {
        out << "0";
    }
    else {
        unsigned long long temp = src.key1;
        std::string binary;
        while (temp > 0) {
            binary = char('0' + (temp & 1)) + binary;
            temp >>= 1;
        }
        out << binary;
    }

    out << ":";

    out << "key2 #c(";
    out << std::fixed << std::setprecision(1) << src.key2.real() << " ";
    out << std::fixed << std::setprecision(1) << src.key2.imag() << "):";

    out << "key3 \"" << src.key3 << "\":)";

    return out;
}