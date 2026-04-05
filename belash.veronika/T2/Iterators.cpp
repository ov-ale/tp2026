#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <iomanip>
#include <cctype>

struct DataStruct
{
    double key1;
    unsigned long long key2;
    std::string key3;
};

class iofmtguard
{
public:
    explicit iofmtguard(std::basic_ios<char>& s) :
        s_(s),
        width_(s.width()),
        precision_(s.precision()),
        fill_(s.fill()),
        fmt_(s.flags())
    {
    }

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
    std::streamsize precision_;
    char fill_;
    std::basic_ios<char>::fmtflags fmt_;
};

std::istream& operator>>(std::istream& in, DataStruct& dest)
{
    std::istream::sentry sentry(in);
    if (!sentry)
    {
        return in;
    }

    iofmtguard fmtguard(in);

    char ch = 0;
    std::string key;

    in >> ch;
    if (ch != '(')
    {
        in.setstate(std::ios::failbit);
        return in;
    }

    in >> ch;
    if (ch != ':')
    {
        in.setstate(std::ios::failbit);
        return in;
    }

    in >> key;
    if (key != "key1")
    {
        in.setstate(std::ios::failbit);
        return in;
    }

    double val1;
    char suffix;
    in >> val1 >> suffix;
    if (suffix != 'd' && suffix != 'D')
    {
        in.setstate(std::ios::failbit);
        return in;
    }

    in >> ch;
    if (ch != ':')
    {
        in.setstate(std::ios::failbit);
        return in;
    }

    in >> key;
    if (key != "key2")
    {
        in.setstate(std::ios::failbit);
        return in;
    }

    unsigned long long val2;
    std::string suffix2;
    in >> val2 >> suffix2;

    for (char& c : suffix2)
        c = std::tolower(static_cast<unsigned char>(c));

    if (suffix2 != "ull")
    {
        in.setstate(std::ios::failbit);
        return in;
    }

    in >> ch;
    if (ch != ':')
    {
        in.setstate(std::ios::failbit);
        return in;
    }

    in >> key;
    if (key != "key3")
    {
        in.setstate(std::ios::failbit);
        return in;
    }
    char quote;
    in >> quote;
    if (quote != '"')
    {
        in.setstate(std::ios::failbit);
        return in;
    }

    std::string val3;
    std::getline(in, val3, '"');
    in >> ch;
    if (ch != ':')
    {
        in.setstate(std::ios::failbit);
        return in;
    }

    in >> ch;
    if (ch != ')')
    {
        in.setstate(std::ios::failbit);
        return in;
    }

    dest.key1 = val1;
    dest.key2 = val2;
    dest.key3 = val3;

    return in;
}

std::ostream& operator<<(std::ostream& out, const DataStruct& src)
{
    std::ostream::sentry sentry(out);
    if (!sentry)
    {
        return out;
    }

    iofmtguard fmtguard(out);
    out << "(:key1 ";
    out << std::fixed;
    out << std::setprecision(1);
    out << src.key1;
    out << "d:key2 ";
    out << src.key2;
    out << "ull:key3 \"";
    out << src.key3;
    out << "\":)";
    return out;
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

int main()
{
    std::vector<DataStruct> vec;
    std::copy(
        std::istream_iterator<DataStruct>(std::cin),
        std::istream_iterator<DataStruct>(),
        std::back_inserter(vec)
    );

    std::sort(vec.begin(), vec.end(), compareDataStruct);

    std::copy(
        vec.begin(),
        vec.end(),
        std::ostream_iterator<DataStruct>(std::cout, "\n")
    );

    return 0;
}
