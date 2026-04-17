// 4 VAR
// DBL SCI
// CHR LIT
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <cctype>


struct DataStruct
{
    double key1;
    char key2;
    std::string key3;
};


struct DelimiterIO
{
    char exp;
};

struct DoubleSciIO
{
    double& ref;
};

struct CharLitIO
{
    char& ref;
};

struct StringIO
{
    std::string& ref;
};


struct LabelIO
{
    std::string exp;
};


class Iofmtguard
{
public:
    Iofmtguard(std::basic_ios<char>& s);
    ~Iofmtguard();
private:
    std::basic_ios<char>& s_;
    std::streamsize width_;
    char fill_;
    std::streamsize precision_;
    std::basic_ios<char>::fmtflags fmt_;
};


std::istream& operator>>(std::istream& in, DelimiterIO&& dest);
std::istream& operator>>(std::istream& in, DoubleSciIO&& dest);
std::istream& operator>>(std::istream& in, CharLitIO&& dest);
std::istream& operator>>(std::istream& in, StringIO&& dest);
std::istream& operator>>(std::istream& in, LabelIO&& dest);
std::istream& operator>>(std::istream& in, DataStruct& dest);
std::ostream& operator<<(std::ostream& out, const DataStruct& dest);


bool compareDataStruct(const DataStruct& a, const DataStruct& b)
{
    const double EPS = 1e-9;

    if (std::abs(a.key1 - b.key1) > EPS)
    {
        return a.key1 < b.key1;
    }

    if (a.key2 != b.key2)
    {
        return a.key2 < b.key2;
    }

    return a.key3.length() < b.key3.length();
}


int main()
{
    std::vector<DataStruct> data;
    std::string input;

    while (std::getline(std::cin, input))
    {
        if (input.empty()) continue;
        std::istringstream iss(input);
        DataStruct temp;
        if (iss >> temp)
        {
            data.push_back(temp);
        }
    }

    std::sort(data.begin(), data.end(), compareDataStruct);

    std::copy(
        data.begin(),
        data.end(),
        std::ostream_iterator<DataStruct>(std::cout, "\n")
    );

    return 0;
}


Iofmtguard::Iofmtguard(std::basic_ios<char>& s)
    : s_(s),
    width_(s.width()),
    fill_(s.fill()),
    precision_(s.precision()),
    fmt_(s.flags())
{
}

Iofmtguard::~Iofmtguard()
{
    s_.width(width_);
    s_.fill(fill_);
    s_.precision(precision_);
    s_.flags(fmt_);
}


std::istream& operator>>(std::istream& in, DelimiterIO&& dest)
{
    std::istream::sentry sentry(in);
    if (!sentry)
    {
        return in;
    }
    char c = '0';
    in >> c;
    if (in && (c != dest.exp))
    {
        in.setstate(std::ios::failbit);
    }
    return in;
}


std::istream& operator>>(std::istream& in, DoubleSciIO&& dest)
{
    std::istream::sentry sentry(in);
    if (!sentry) return in;

    std::string token;
    in >> token;

    bool hasExponent = (token.find('e') != std::string::npos) ||
        (token.find('E') != std::string::npos);
    bool hasDot = token.find('.') != std::string::npos;

    if (!hasExponent || !hasDot)
    {
        in.setstate(std::ios::failbit);
        return in;
    }

    try {
        dest.ref = std::stod(token);
    }
    catch (...) {
        in.setstate(std::ios::failbit);
    }

    return in;
}



std::istream& operator>>(std::istream& in, CharLitIO&& dest)
{
    std::istream::sentry sentry(in);
    if (!sentry)
    {
        return in;
    }
    char c = '0';
    in >> DelimiterIO{ '\'' } >> c >> DelimiterIO{ '\'' };
    if (in)
    {
        dest.ref = c;
    }
    return in;
}

std::istream& operator>>(std::istream& in, StringIO&& dest)
{
    std::istream::sentry sentry(in);
    if (!sentry)
    {
        return in;
    }
    return std::getline(in >> DelimiterIO{ '"' }, dest.ref, '"');
}


std::istream& operator>>(std::istream& in, LabelIO&& dest)
{
    std::istream::sentry sentry(in);
    if (!sentry)
    {
        return in;
    }
    std::string data = "";
    if ((in >> StringIO{ data }) && (data != dest.exp))
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
    bool hasKey1 = false;
    bool hasKey2 = false;
    bool hasKey3 = false;

    in >> DelimiterIO{ '(' } >> DelimiterIO{ ':' };

    while (in && !(hasKey1 && hasKey2 && hasKey3))
    {
        std::string key;
        if (!(in >> LabelIO{ key })) break;

        if (key == "key1")
        {
            in >> DelimiterIO{ ' ' } >> DoubleSciIO{ input.key1 };
            hasKey1 = true;
        }
        else if (key == "key2")
        {
            in >> DelimiterIO{ ' ' } >> CharLitIO{ input.key2 };
            hasKey2 = true;
        }
        else if (key == "key3")
        {
            in >> DelimiterIO{ ' ' } >> StringIO{ input.key3 };
            hasKey3 = true;
        }
        else
        {
            in.setstate(std::ios::failbit);
            return in;
        }

        in >> DelimiterIO{ ':' };
    }

    in >> DelimiterIO{ ')' };

    if (hasKey1 && hasKey2 && hasKey3)
    {
        dest = std::move(input);
    }
    else
    {
        in.setstate(std::ios::failbit);
    }
    return in;
}

std::string formatDoubleSci(double value)
{
    if (value == 0.0)
    {
        return "0.0e+0";
    }

    double mantissa = value;
    int exponent = 0;

    if (std::abs(value) < 1.0)
    {
        while (std::abs(mantissa) < 1.0 && exponent > -100)
        {
            mantissa *= 10.0;
            exponent--;
        }
    }
    else
    {
        while (std::abs(mantissa) >= 10.0 && exponent < 100)
        {
            mantissa /= 10.0;
            exponent++;
        }
    }

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1) << mantissa;
    std::string mantissaStr = oss.str();

    if (mantissaStr.back() == '0')
    {
        mantissaStr.pop_back();
        if (mantissaStr.back() == '.')
        {
            mantissaStr.pop_back();
        }
    }

    std::string result = mantissaStr + "e";
    if (exponent >= 0) result += "+";
    result += std::to_string(exponent);
    for (char& c : result)
    {
        c = std::tolower(static_cast<unsigned char>(c));
    }

    return result;
}


std::string formatCharLit(char c)
{
    switch (c)
    {
    case '\n': return "'\\n'";
    case '\t': return "'\\t'";
    case '\\': return "'\\\\'";
    case '\'': return "'\\''";
    default: return std::string("'") + c + "'";
    }
}


std::ostream& operator<<(std::ostream& out, const DataStruct& src)
{
    std::ostream::sentry sentry(out);
    if (!sentry)
    {
        return out;
    }

    Iofmtguard fmtguard(out);

    out << "(:key1 " << formatDoubleSci(src.key1)
        << ":key2 " << formatCharLit(src.key2)
        << ":key3 \"" << src.key3 << "\":)";
    return out;
}
