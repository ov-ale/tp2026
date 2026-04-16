#include <iostream>
#include <sstream>
#include <string>
#include <cassert>
#include <iterator>
#include <vector>
#include <iomanip>
#include <complex>
#include <algorithm>

struct DataStruct
{
    unsigned long long key1;
    std::complex<double> key2;
    std::string key3;
};

struct DelimiterIO
{
    char exp;
};

struct UllIO
{
    unsigned long long& ref;
};

struct CmpIO
{
    std::complex<double>& ref;
};

struct StrIO
{
    std::string& ref;
};

class iofmtguard
{
public:
    explicit iofmtguard(std::basic_ios<char>& s);
    ~iofmtguard();
private:
    std::basic_ios< char >& s_;
    std::streamsize width_;
    char fill_;
    std::streamsize precision_;
    std::basic_ios< char >::fmtflags fmt_;
};

std::istream& operator>>(std::istream& in, DelimiterIO&& dest);
std::istream& operator>>(std::istream& in, UllIO&& dest);
std::istream& operator>>(std::istream& in, CmpIO&& dest);
std::istream& operator>>(std::istream& in, StrIO&& dest);
std::istream& operator>>(std::istream& in, DataStruct& dest);
std::ostream& operator<<(std::ostream& out, const DataStruct& dest);

bool compareDataStruct(const DataStruct& a, const DataStruct& b);

int main()
{
    std::vector<DataStruct> data;

    std::string line;
    while (std::getline(std::cin, line))
    {
        if (line.empty()) continue;

        std::istringstream iss(line);
        DataStruct temp;

        if (iss >> temp)
        {
            data.push_back(temp);
        }
    }

    std::sort(data.begin(), data.end(), compareDataStruct);

    std::copy(data.begin(), data.end(), std::ostream_iterator<DataStruct>(std::cout, "\n"));

    return 0;
}

bool compareDataStruct(const DataStruct& a, const DataStruct& b)
{
    if (a.key1 != b.key1)
    {
        return a.key1 < b.key1;
    }

    double absA = std::abs(a.key2);
    double absB = std::abs(b.key2);
    const double EPSILON = 1e-9;

    if (std::abs(absA - absB) > EPSILON)
    {
        return absA < absB;
    }

    return a.key3.length() < b.key3.length();
}

std::istream& operator>>(std::istream& in, DelimiterIO&& dest)
{
    std::istream::sentry sentry(in, false);
    if (!sentry) return in;

    char c = in.get();
    if (in && c != dest.exp)
    {
        in.setstate(std::ios::failbit);
    }

    return in;
}

std::istream& operator>>(std::istream& in, UllIO&& dest)
{
    std::istream::sentry sentry(in);
    if (!sentry) return in;

    if (!(in >> dest.ref))
    {
        return in;
    }

    char c1, c2, c3;
    if (!in.get(c1) || !in.get(c2) || !in.get(c3))
    {
        in.setstate(std::ios::failbit);
        return in;
    }

    bool valid = ((c1 == 'u' && c2 == 'l' && c3 == 'l') || (c1 == 'U' && c2 == 'L' && c3 == 'L'));
    if (!valid)
    {
        in.setstate(std::ios::failbit);
    }

    return in;
}

std::istream& operator>>(std::istream& in, CmpIO&& dest)
{
    std::istream::sentry sentry(in);
    if (!sentry) return in;

    double re = 0.0, im = 0.0;

    in >> DelimiterIO{ '#' } >> DelimiterIO{ 'c' } >> DelimiterIO{ '(' } >> re >> im >> DelimiterIO{ ')' };

    if (in)
    {
        dest.ref = std::complex<double>(re, im);
    }

    return in;
}

std::istream& operator>>(std::istream& in, StrIO&& dest)
{
    std::istream::sentry sentry(in);
    if (!sentry) return in;

    if (!(in >> DelimiterIO{ '"' }))
    {
        return in;
    }

    std::getline(in, dest.ref, '"');
    if (!in || in.eof())
    {
        in.setstate(std::ios::failbit);
    }

    return in;
}

std::istream& operator>>(std::istream& in, DataStruct& dest)
{
    std::istream::sentry sentry(in);
    if (!sentry) return in;

    DataStruct input;
    bool hasKey1 = false, hasKey2 = false, hasKey3 = false;

    if (!(in >> DelimiterIO{ '(' } >> DelimiterIO{ ':' })) {
        in.setstate(std::ios::failbit);
        return in;
    }

    for (size_t i = 0; i < 3; ++i) {
        std::string key;
        if (!(in >> key)) {
            in.setstate(std::ios::failbit);
            return in;
        }

        if (key == "key1")
        {
            if (hasKey1)
            {
                in.setstate(std::ios::failbit);
                return in;
            }
            if (!(in >> UllIO{ input.key1 })) return in;
            hasKey1 = true;
        }
        else if (key == "key2") {
            if (hasKey2)
            {
                in.setstate(std::ios::failbit);
                return in;
            }
            if (!(in >> CmpIO{ input.key2 })) return in;
            hasKey2 = true;
        }
        else if (key == "key3")
        {
            if (hasKey3)
            {
                in.setstate(std::ios::failbit);
                return in;
            }
            if (!(in >> StrIO{ input.key3 })) return in;
            hasKey3 = true;
        }
        else
        {
            in.setstate(std::ios::failbit);
            return in;
        }

        if (i < 2)
        {
            if (!(in >> DelimiterIO{ ':' })) {
                in.setstate(std::ios::failbit);
                return in;
            }
        }
    }

    if (!(in >> DelimiterIO{ ':' } >> DelimiterIO{ ')' }) || (!hasKey1 || !hasKey2 || !hasKey3))
    {
        in.setstate(std::ios::failbit);
        return in;
    }

    dest = std::move(input);
    return in;
}

std::ostream& operator<<(std::ostream& out, const DataStruct& src)
{
    std::ostream::sentry sentry(out);
    if (!sentry) return out;

    iofmtguard guard(out);
    out << "(:key1 " << src.key1 << "ULL"
        << ":key2 #c(" << std::fixed << std::setprecision(1)
        << src.key2.real() << " " << src.key2.imag() << ")"
        << ":key3 \"" << src.key3 << "\":)";
    return out;
}

iofmtguard::iofmtguard(std::basic_ios<char>& s) :
    s_(s),
    width_(s.width()),
    fill_(s.fill()),
    precision_(s.precision()),
    fmt_(s.flags())
{
}

iofmtguard::~iofmtguard()
{
    s_.width(width_);
    s_.fill(fill_);
    s_.precision(precision_);
    s_.flags(fmt_);
}
