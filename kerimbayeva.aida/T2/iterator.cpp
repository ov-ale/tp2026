#include <iostream>
#include <sstream>
#include <string>
#include <iterator>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <cctype>

struct DataStruct
{
    unsigned long long key1;
    unsigned long long key2;
    std::string key3;
};

struct DelimiterIO
{
    char exp;
};

struct UllLitIO
{
    unsigned long long& ref;
};

struct UllBinIO
{
    unsigned long long& ref;
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
    std::basic_ios<char>& s_;
    std::streamsize width_;
    char fill_;
    std::streamsize precision_;
    std::basic_ios<char>::fmtflags fmt_;
};

std::istream& operator>>(std::istream& in, DelimiterIO&& dest);
std::istream& operator>>(std::istream& in, UllLitIO&& dest);
std::istream& operator>>(std::istream& in, UllBinIO&& dest);
std::istream& operator>>(std::istream& in, StrIO&& dest);
std::istream& operator>>(std::istream& in, DataStruct& dest);
std::ostream& operator<<(std::ostream& out, const DataStruct& dest);

bool compareDataStruct(const DataStruct& a, const DataStruct& b);

int main()
{
    std::vector<DataStruct> data;

    std::copy(
        std::istream_iterator<DataStruct>(std::cin),
        std::istream_iterator<DataStruct>(),
        std::back_inserter(data)
    );

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
    if (a.key2 != b.key2)
    {
        return a.key2 < b.key2;
    }
    return a.key3.length() < b.key3.length();
}

std::istream& operator>>(std::istream& in, DelimiterIO&& dest)
{
    std::istream::sentry sentry(in);
    if (!sentry) return in;

    char c = '0';
    in >> c;
    if (in && c != dest.exp)
    {
        in.setstate(std::ios::failbit);
    }
    return in;
}

std::istream& operator>>(std::istream& in, UllLitIO&& dest)
{
    std::istream::sentry sentry(in);
    if (!sentry) return in;

    std::string num;
    in >> num;

    if (!in) return in;

    bool hasSuffix = false;
    if (num.size() >= 3)
    {
        std::string suffix = num.substr(num.size() - 3);
        if (suffix == "ULL" || suffix == "ull")
        {
            hasSuffix = true;
            num = num.substr(0, num.size() - 3);
        }
    }

    if (!hasSuffix)
    {
        in.setstate(std::ios::failbit);
        return in;
    }

    for (char c : num)
    {
        if (!std::isdigit(static_cast<unsigned char>(c)))
        {
            in.setstate(std::ios::failbit);
            return in;
        }
    }

    try
    {
        dest.ref = std::stoull(num);
    }
    catch (...)
    {
        in.setstate(std::ios::failbit);
    }
    return in;
}

std::istream& operator>>(std::istream& in, UllBinIO&& dest)
{
    std::istream::sentry sentry(in);
    if (!sentry) return in;

    char c1, c2;
    if (!(in >> c1 >> c2) || c1 != '0' || std::tolower(static_cast<unsigned char>(c2)) != 'b')
    {
        in.setstate(std::ios::failbit);
        return in;
    }

    std::string binary;
    while (in.peek() == '0' || in.peek() == '1')
    {
        binary += static_cast<char>(in.get());
    }

    if (binary.empty())
    {
        in.setstate(std::ios::failbit);
        return in;
    }

    try
    {
        dest.ref = std::stoull(binary, nullptr, 2);
    }
    catch (...)
    {
        in.setstate(std::ios::failbit);
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
    return in;
}

std::istream& operator>>(std::istream& in, DataStruct& dest)
{
    std::istream::sentry sentry(in);
    if (!sentry) return in;

    DataStruct input;
    bool hasKey1 = false, hasKey2 = false, hasKey3 = false;

    if (!(in >> DelimiterIO{ '(' } >> DelimiterIO{ ':' }))
    {
        in.setstate(std::ios::failbit);
        return in;
    }

    for (int i = 0; i < 3; ++i)
    {
        std::string key;
        if (!(in >> key))
        {
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
            if (!(in >> UllLitIO{ input.key1 })) return in;
            hasKey1 = true;
        }
        else if (key == "key2")
        {
            if (hasKey2)
            {
                in.setstate(std::ios::failbit);
                return in;
            }
            if (!(in >> UllBinIO{ input.key2 })) return in;
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
            if (!(in >> DelimiterIO{ ':' }))
            {
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

void printBinary(std::ostream& out, unsigned long long val)
{
    out << "0b";
    if (val == 0)
    {
        out << "0";
        return;
    }
    std::string binary;
    while (val > 0)
    {
        binary = char('0' + (val % 2)) + binary;
        val /= 2;
    }
    out << binary;
}

std::ostream& operator<<(std::ostream& out, const DataStruct& src)
{
    std::ostream::sentry sentry(out);
    if (!sentry) return out;

    iofmtguard guard(out);
    out << "(:key1 " << src.key1 << "ull:key2 ";
    printBinary(out, src.key2);
    out << ":key3 \"" << src.key3 << "\":)";
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
