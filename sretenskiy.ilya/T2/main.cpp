#include <iostream>
#include <vector>
#include <iterator>
#include <string>
#include <algorithm>
#include <cmath>
#include <cctype>
#include <iomanip>

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

struct CharIO
{
    char& ref;
};

struct DoubleIO
{
    double& ref;
};

struct StringIO
{
    std::string& ref;
};

class iofmtguard
{
public:
    iofmtguard(std::basic_ios<char>& s) :
        s_(s),
        width_(s.width()),
        fill_(s.fill()),
        precision_(s.precision()),
        fmt_(s.flags())
    {}

    ~iofmtguard()
    {
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

const double EPSILON = 1e-9;

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

std::istream& operator>>(std::istream& in, DoubleIO&& dest)
{
    std::istream::sentry sentry(in);
    if (!sentry)
    {
        return in;
    }

    std::string mantissaStr;
    char exponentSign = '+';
    int exponent = 0;
    char symbol;
    bool hasDecimalPoint = false;
    bool validMantissa = false;
    bool hasDigitAfterPoint = false;
    char sign = '+';

    if (in.peek() == '-')
    {
        in.get(sign);
    }

    while (in.get(symbol))
    {
        if (std::isdigit(symbol))
        {
            mantissaStr += symbol;
            validMantissa = true;
            if (hasDecimalPoint)
            {
                hasDigitAfterPoint = true;
            }
        }
        else if (symbol == '.' && !hasDecimalPoint)
        {
            if (mantissaStr.empty())
            {
                in.setstate(std::ios::failbit);
                return in;
            }
            mantissaStr += symbol;
            hasDecimalPoint = true;
        }
        else if (symbol == 'e' || symbol == 'E')
        {
            break;
        }
        else
        {
            in.putback(symbol);
            in.setstate(std::ios::failbit);
            return in;
        }
    }

    if (!validMantissa || !hasDecimalPoint || !hasDigitAfterPoint)
    {
        in.setstate(std::ios::failbit);
        return in;
    }

    if (!in.get(exponentSign) || (exponentSign != '+' && exponentSign != '-'))
    {
        in.setstate(std::ios::failbit);
        return in;
    }

    in >> exponent;
    if (!in)
    {
        in.setstate(std::ios::failbit);
        return in;
    }

    double mantissa = std::stod(mantissaStr);
    int finalExponent = (exponentSign == '-') ? -exponent : exponent;

    if (sign == '-')
    {
        mantissa = -mantissa;
    }

    dest.ref = mantissa * std::pow(10.0, finalExponent);
    return in;
}

std::istream& operator>>(std::istream& in, CharIO&& dest)
{
    std::istream::sentry sentry(in);
    if (!sentry)
    {
        return in;
    }
    return in >> DelimiterIO{ '\'' } >> dest.ref >> DelimiterIO{ '\'' };
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

std::istream& operator>>(std::istream& in, DataStruct& dest)
{
    std::istream::sentry sentry(in);
    if (!sentry)
    {
        return in;
    }

    DataStruct temp;
    bool foundKey1 = false, foundKey2 = false, foundKey3 = false;

    in >> DelimiterIO{ '(' };
    if (!in)
    {
        in.setstate(std::ios::failbit);
        return in;
    }

    while (in)
    {
        if (in.peek() == ':')
        {
            char c;
            in >> c;
            if (in.peek() == ')')
            {
                in >> c;
                break;
            }
            continue;
        }

        std::string label;
        if (!(in >> label))
        {
            break;
        }

        if (label == "key1")
        {
            if (!(in >> DoubleIO{ temp.key1 })) break;
            foundKey1 = true;
        }
        else if (label == "key2")
        {
            if (!(in >> CharIO{ temp.key2 })) break;
            foundKey2 = true;
        }
        else if (label == "key3")
        {
            if (!(in >> StringIO{ temp.key3 })) break;
            foundKey3 = true;
        }
        else
        {
            in.setstate(std::ios::failbit);
            break;
        }
    }

    if (!foundKey1 || !foundKey2 || !foundKey3)
    {
        in.setstate(std::ios::failbit);
        return in;
    }

    dest = temp;
    return in;
}

bool comparator(const DataStruct& a, const DataStruct& b)
{
    if (std::abs(a.key1 - b.key1) > EPSILON)
    {
        return a.key1 < b.key1;
    }
    if (a.key2 != b.key2)
    {
        return a.key2 < b.key2;
    }
    return a.key3.length() < b.key3.length();
}

std::ostream& operator<<(std::ostream& out, const DataStruct& data)
{
    std::ostream::sentry sentry(out);
    if (!sentry)
    {
        return out;
    }
    iofmtguard fmtguard(out);

    double mantissa = data.key1;
    int exponent = 0;

    if (mantissa == 0.0)
    {
        out << "(:key1 0.0e+0:key2 '" << data.key2 << "':key3 \"" << data.key3 << "\":)";
        return out;
    }

    bool negative = mantissa < 0;
    mantissa = std::abs(mantissa);

    while (mantissa < 1.0)
    {
        mantissa *= 10.0;
        exponent--;
    }

    while (mantissa >= 10.0)
    {
        mantissa /= 10.0;
        exponent++;
    }

    if (negative)
    {
        mantissa = -mantissa;
    }

    out << "(:key1 ";
    out << std::fixed << std::setprecision(1) << mantissa;
    out << "e" << (exponent >= 0 ? "+" : "") << exponent;
    out << ":key2 '" << data.key2 << "':key3 \"" << data.key3 << "\":)";

    return out;
}

int main()
{
    std::vector<DataStruct> data;

    std::copy(std::istream_iterator<DataStruct>(std::cin), std::istream_iterator<DataStruct>(), std::back_inserter(data));
    std::sort(data.begin(), data.end(), comparator);
    std::copy(data.begin(), data.end(), std::ostream_iterator<DataStruct>(std::cout, "\n"));
    return 0;
}

