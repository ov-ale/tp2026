#include <iostream>
#include <vector>
#include <iterator>
#include <string>
#include <algorithm>
#include <cmath>
#include <cctype>
#include <iomanip>
#include <sstream>

struct DataStruct
{
    double key1 = 0.0;
    char key2 = 0;
    std::string key3;
};

struct DoubleIO
{
    double& ref;
};

struct CharIO
{
    char& ref;
};

struct StringIO
{
    std::string& ref;
};

const double EPSILON = 1e-9;

std::istream& operator>>(std::istream& in, DoubleIO&& dest)
{
    std::istream::sentry sentry(in);
    if (!sentry) return in;

    if (in.peek() == '0')
    {
        std::streampos pos = in.tellg();
        std::string test;
        if (in >> test && (test == "0.0" || test == "0"))
        {
            dest.ref = 0.0;
            return in;
        }
        in.clear();
        in.seekg(pos);
    }

    std::string numStr;
    char c;
    bool hasDot = false;
    bool hasDigitsBefore = false;
    bool hasDigitsAfter = false;
    bool hasExp = false;

    if (in.peek() == '+' || in.peek() == '-')
    {
        numStr += (char)in.get();
    }

    while (in.get(c))
    {
        if (std::isdigit(c))
        {
            numStr += c;
            if (hasDot) hasDigitsAfter = true;
            else hasDigitsBefore = true;
        }
        else if (c == '.')
        {
            if (hasDot) { in.setstate(std::ios::failbit); return in; }
            hasDot = true;
            numStr += c;
        }
        else if (c == 'e' || c == 'E')
        {
            hasExp = true;
            numStr += c;
            break;
        }
        else
        {
            in.putback(c);
            in.setstate(std::ios::failbit);
            return in;
        }
    }

    if (!hasDot || !hasDigitsBefore || !hasDigitsAfter || !hasExp)
    {
        in.setstate(std::ios::failbit);
        return in;
    }

    if (in.peek() == '+' || in.peek() == '-')
    {
        numStr += (char)in.get();
    }

    bool hasExpDigits = false;
    while (in && std::isdigit(in.peek()))
    {
        numStr += (char)in.get();
        hasExpDigits = true;
    }

    if (!hasExpDigits)
    {
        in.setstate(std::ios::failbit);
        return in;
    }

    try
    {
        dest.ref = std::stod(numStr);
    }
    catch (...)
    {
        in.setstate(std::ios::failbit);
    }

    return in;
}

std::istream& operator>>(std::istream& in, CharIO&& dest)
{
    std::istream::sentry sentry(in);
    if (!sentry) return in;
    char quote1, quote2;
    in >> quote1;
    if (quote1 != '\'') { in.setstate(std::ios::failbit); return in; }
    in >> std::noskipws >> dest.ref >> std::skipws;
    in >> quote2;
    if (quote2 != '\'') { in.setstate(std::ios::failbit); return in; }
    return in;
}

std::istream& operator>>(std::istream& in, StringIO&& dest)
{
    std::istream::sentry sentry(in);
    if (!sentry) return in;
    char quote;
    in >> quote;
    if (quote != '"') { in.setstate(std::ios::failbit); return in; }
    std::getline(in, dest.ref, '"');
    return in;
}

std::istream& operator>>(std::istream& in, DataStruct& dest)
{
    std::istream::sentry sentry(in);
    if (!sentry) return in;

    while (std::isspace(in.peek())) in.get();

    char openBracket;
    if (!(in >> openBracket) || openBracket != '(')
    {
        in.setstate(std::ios::failbit);
        return in;
    }

    DataStruct temp;
    bool foundKey1 = false, foundKey2 = false, foundKey3 = false;
    bool isValid = true;

    while (in)
    {
        while (std::isspace(in.peek())) in.get();

        if (in.peek() == ')') break;
        if (in.peek() != ':') { isValid = false; break; }
        in.get();

        if (in.peek() == ')') break;

        std::string label;
        while (in && (std::isalpha(in.peek()) || std::isdigit(in.peek())))
        {
            label += (char)in.get();
        }

        if (in.peek() != ' ') { isValid = false; break; }
        in.get();

        if (label == "key1")
        {
            if (foundKey1 || !(in >> DoubleIO{ temp.key1 })) { isValid = false; break; }
            foundKey1 = true;
        }
        else if (label == "key2")
        {
            if (foundKey2 || !(in >> CharIO{ temp.key2 })) { isValid = false; break; }
            foundKey2 = true;
        }
        else if (label == "key3")
        {
            if (foundKey3 || !(in >> StringIO{ temp.key3 })) { isValid = false; break; }
            foundKey3 = true;
        }
        else
        {
            isValid = false; break;
        }
    }

    while (std::isspace(in.peek())) in.get();

    char closingBracket;
    if (isValid && foundKey1 && foundKey2 && foundKey3 && (in >> closingBracket) && closingBracket == ')')
    {
        dest = temp;
        return in;
    }

    in.setstate(std::ios::failbit);
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
    if (!sentry) return out;

    std::ostringstream oss;
    oss << std::scientific << std::nouppercase << std::setprecision(1) << data.key1;
    std::string sciStr = oss.str();

    size_t ePos = sciStr.find('e');
    if (ePos != std::string::npos)
    {
        size_t signPos = ePos + 1;
        size_t firstDigitPos = signPos + 1;
        while (firstDigitPos < sciStr.length() - 1 && sciStr[firstDigitPos] == '0')
        {
            sciStr.erase(firstDigitPos, 1);
        }
    }

    out << "(:key1 " << sciStr << ":key2 '" << data.key2 << "':key3 \"" << data.key3 << "\":)";
    return out;
}

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

    if (data.empty())
    {
        return 0;
    }

    std::sort(data.begin(), data.end(), comparator);

    std::copy(data.begin(), data.end(),
        std::ostream_iterator<DataStruct>(std::cout, "\n"));

    return 0;
}
