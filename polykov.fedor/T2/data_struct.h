#ifndef DATA_STRUCT_H
#define DATA_STRUCT_H

#include <iostream>
#include <string>
#include <iomanip>
#include <cctype>
#include <sstream>

struct DataStruct
{
    double key1;
    unsigned long long key2;
    std::string key3;

    DataStruct() : key1(0.0), key2(0), key3() {}
};

struct DelimiterIO
{
    char exp;
};

struct DoubleLitIO
{
    double& ref;
};

struct UllHexIO
{
    unsigned long long& ref;
};

struct StringIO
{
    std::string& ref;
};

struct LabelIO
{
    std::string exp;
};

class iofmtguard
{
private:
    std::basic_ios<char>& s_;
    std::streamsize width_;
    char fill_;
    std::streamsize precision_;
    std::basic_ios<char>::fmtflags fmt_;

public:
    explicit iofmtguard(std::basic_ios<char>& s) :
        s_(s),
        width_(s.width()),
        fill_(s.fill()),
        precision_(s.precision()),
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
};

std::istream& operator>>(std::istream& in, DelimiterIO&& dest)
{
    std::istream::sentry sentry(in);
    if (!sentry)
    {
        return in;
    }
    char c = in.get();
    if (in && c != dest.exp)
    {
        in.setstate(std::ios::failbit);
    }
    return in;
}

std::istream& operator>>(std::istream& in, DoubleLitIO&& dest)
{
    std::istream::sentry sentry(in);
    if (!sentry)
    {
        return in;
    }

    in >> std::ws;
    std::string token;
    char c;
    while (in.peek() != EOF && !std::isspace(in.peek()) && in.peek() != ':')
    {
        in.get(c);
        token += c;
    }

    if (token.empty())
    {
        in.setstate(std::ios::failbit);
        return in;
    }
    int next = in.peek();
    if (next != ':' && next != EOF)
    {
        in.setstate(std::ios::failbit);
        return in;
    }

    if (token.size() < 2 || (token.back() != 'd' && token.back() != 'D'))
    {
        in.setstate(std::ios::failbit);
        return in;
    }

    std::string numberPart = token.substr(0, token.size() - 1);
    size_t dotPos = numberPart.find('.');
    if (dotPos == std::string::npos)
    {
        in.setstate(std::ios::failbit);
        return in;
    }

    if (dotPos == 0 || dotPos == numberPart.size() - 1)
    {
        in.setstate(std::ios::failbit);
        return in;
    }

    for (size_t i = 0; i < numberPart.size(); ++i)
    {
        if (i == 0 && numberPart[i] == '-') continue;
        if (i == dotPos) continue;
        if (!std::isdigit(numberPart[i]))
        {
            in.setstate(std::ios::failbit);
            return in;
        }
    }

    try
    {
        dest.ref = std::stod(numberPart);
    }
    catch (...)
    {
        in.setstate(std::ios::failbit);
    }

    return in;
}

std::istream& operator>>(std::istream& in, UllHexIO&& dest)
{
    std::istream::sentry sentry(in);
    if (!sentry)
    {
        return in;
    }

    in >> std::ws;
    char prefix[2] = { 0 };
    if (!in.get(prefix[0]) || !in.get(prefix[1]))
    {
        in.setstate(std::ios::failbit);
        return in;
    }
    if (prefix[0] != '0' || (prefix[1] != 'x' && prefix[1] != 'X'))
    {
        in.setstate(std::ios::failbit);
        return in;
    }

    std::string hexStr;
    char c;
    while (std::isxdigit(in.peek()))
    {
        in.get(c);
        hexStr += c;
    }

    if (hexStr.empty())
    {
        in.setstate(std::ios::failbit);
        return in;
    }

    int next = in.peek();
    if (next != ':' && next != EOF)
    {
        in.setstate(std::ios::failbit);
        return in;
    }

    try
    {
        dest.ref = std::stoull(hexStr, nullptr, 16);
    }
    catch (...)
    {
        in.setstate(std::ios::failbit);
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
    if (!(in >> DelimiterIO{ '"' }))
    {
        return in;
    }
    std::getline(in, dest.ref, '"');
    if (!in)
    {
        in.setstate(std::ios::failbit);
    }
    return in;
}

std::istream& operator>>(std::istream& in, LabelIO&& dest)
{
    std::istream::sentry sentry(in);
    if (!sentry)
    {
        return in;
    }

    std::string data;
    char c;
    if (!in.get(c) || std::isspace(c))
    {
        in.setstate(std::ios::failbit);
        return in;
    }
    data += c;
    while (in.get(c) && !std::isspace(c))
    {
        data += c;
    }
    if (data != dest.exp)
    {
        in.setstate(std::ios::failbit);
    }
    return in;
}

std::istream& operator>>(std::istream& in, DataStruct& dest)
{
    std::istream::sentry sentry(in);
    if (!sentry)
    {
        return in;
    }

    std::string line;
    while (std::getline(in, line))
    {
        if (line.empty()) continue;

        std::istringstream iss(line);
        if (!(iss >> DelimiterIO{ '(' } >> DelimiterIO{ ':' }))
        {
            continue;
        }

        DataStruct input;
        bool hasKey1 = false, hasKey2 = false, hasKey3 = false;
        int fieldsRead = 0;
        bool error = false;

        std::string label;
        char ch;
        if (!iss.get(ch) || std::isspace(ch))
        {
            continue;
        }
        label += ch;
        while (iss.get(ch) && !std::isspace(ch))
        {
            label += ch;
        }

        if (label == "key1" && !hasKey1)
        {
            if (iss >> DoubleLitIO{ input.key1 })
            {
                hasKey1 = true;
                ++fieldsRead;
            }
            else error = true;
        }
        else if (label == "key2" && !hasKey2)
        {
            if (iss >> UllHexIO{ input.key2 })
            {
                hasKey2 = true;
                ++fieldsRead;
            }
            else error = true;
        }
        else if (label == "key3" && !hasKey3)
        {
            if (iss >> StringIO{ input.key3 })
            {
                hasKey3 = true;
                ++fieldsRead;
            }
            else error = true;
        }
        else
        {
            continue;
        }

        if (error) continue;

        while (fieldsRead < 3 && !error)
        {
            if (!(iss >> DelimiterIO{ ':' }))
            {
                error = true;
                break;
            }

            std::string label2;
            char ch2;
            if (!iss.get(ch2) || std::isspace(ch2))
            {
                error = true;
                break;
            }
            label2 += ch2;
            while (iss.get(ch2) && !std::isspace(ch2))
            {
                label2 += ch2;
            }

            if (label2 == "key1" && !hasKey1)
            {
                if (iss >> DoubleLitIO{ input.key1 })
                {
                    hasKey1 = true;
                    ++fieldsRead;
                }
                else error = true;
            }
            else if (label2 == "key2" && !hasKey2)
            {
                if (iss >> UllHexIO{ input.key2 })
                {
                    hasKey2 = true;
                    ++fieldsRead;
                }
                else error = true;
            }
            else if (label2 == "key3" && !hasKey3)
            {
                if (iss >> StringIO{ input.key3 })
                {
                    hasKey3 = true;
                    ++fieldsRead;
                }
                else error = true;
            }
            else
            {
                error = true;
            }
        }

        if (error || !hasKey1 || !hasKey2 || !hasKey3)
        {
            continue;
        }

        if (iss >> DelimiterIO{ ':' } >> DelimiterIO{ ')' })
        {
            dest = std::move(input);
            return in;
        }
    }

    in.setstate(std::ios::failbit);
    return in;
}

std::ostream& operator<<(std::ostream& out, const DataStruct& dest)
{
    std::ostream::sentry sentry(out);
    if (!sentry)
    {
        return out;
    }
    iofmtguard guard(out);
    out << "(:key1 " << std::fixed << std::setprecision(1) << dest.key1 << "d";
    out << ":key2 0x" << std::hex << std::uppercase << dest.key2;
    out << ":key3 \"" << dest.key3 << "\":)";
    return out;
}

struct DataStructComparator
{
    bool operator()(const DataStruct& lhs, const DataStruct& rhs) const
    {
        if (lhs.key1 != rhs.key1)
        {
            return lhs.key1 < rhs.key1;
        }
        if (lhs.key2 != rhs.key2)
        {
            return lhs.key2 < rhs.key2;
        }
        return lhs.key3.length() < rhs.key3.length();
    }
};

#endif