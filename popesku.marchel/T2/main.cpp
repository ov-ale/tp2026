#include <algorithm>
#include <cctype>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

struct DataStruct
{
    unsigned long long key1;
    unsigned long long key2;
    std::string key3;
};

bool parseULLLiteral(const std::string& str, unsigned long long& result)
{
    if (str.size() < 4)
    {
        return false;
    }

    std::string suffix = str.substr(str.size() - 3);
    if (suffix != "ull" && suffix != "ULL")
    {
        return false;
    }

    std::string numberPart = str.substr(0, str.size() - 3);
    if (numberPart.empty())
    {
        return false;
    }

    for (char c : numberPart)
    {
        if (!std::isdigit(c))
        {
            return false;
        }
    }

    try
    {
        result = std::stoull(numberPart);
    }
    catch (...)
    {
        return false;
    }

    return true;
}

bool parseULLBinary(const std::string& str, unsigned long long& result)
{
    if (str.size() < 3)
    {
        return false;
    }

    if (str[0] != '0' || (str[1] != 'b' && str[1] != 'B'))
    {
        return false;
    }

    result = 0;

    for (size_t i = 2; i < str.size(); ++i)
    {
        char c = str[i];

        if (c != '0' && c != '1')
        {
            return false;
        }

        unsigned long long bit = (c == '1') ? 1 : 0;
        result = result * 2 + bit;
    }

    return true;
}

bool parseString(const std::string& str, std::string& result)
{
    if (str.size() < 2)
    {
        return false;
    }

    if (str.front() != '"' || str.back() != '"')
    {
        return false;
    }

    result = str.substr(1, str.size() - 2);
    return true;
}

bool parseRecord(const std::string& line, DataStruct& result)
{
    if (line.size() < 2 || line.front() != '(' || line.back() != ')')
    {
        return false;
    }

    std::string content = line.substr(1, line.size() - 2);

    unsigned long long key1 = 0;
    unsigned long long key2 = 0;
    std::string key3;

    bool hasKey1 = false;
    bool hasKey2 = false;
    bool hasKey3 = false;

    size_t pos = 0;

    while (pos < content.size())
    {
        if (content[pos] != ':')
        {
            return false;
        }

        ++pos;

        if (pos == content.size())
        {
            break;
        }

        size_t keyStart = pos;
        while (pos < content.size() &&
               std::isalnum(static_cast<unsigned char>(content[pos])))
        {
            ++pos;
        }

        if (keyStart == pos)
        {
            return false;
        }

        std::string key = content.substr(keyStart, pos - keyStart);

        if (pos >= content.size() || content[pos] != ' ')
        {
            return false;
        }

        ++pos;

        if (key == "key1")
        {
            if (hasKey1)
            {
                return false;
            }

            size_t valueStart = pos;
            while (pos < content.size() && content[pos] != ':')
            {
                ++pos;
            }

            if (pos >= content.size())
            {
                return false;
            }

            std::string token = content.substr(valueStart, pos - valueStart);

            if (!parseULLLiteral(token, key1))
            {
                return false;
            }

            hasKey1 = true;
        }
        else if (key == "key2")
        {
            if (hasKey2)
            {
                return false;
            }

            size_t valueStart = pos;
            while (pos < content.size() && content[pos] != ':')
            {
                ++pos;
            }

            if (pos >= content.size())
            {
                return false;
            }

            std::string token = content.substr(valueStart, pos - valueStart);

            if (!parseULLBinary(token, key2))
            {
                return false;
            }

            hasKey2 = true;
        }
        else if (key == "key3")
        {
            if (hasKey3)
            {
                return false;
            }

            if (pos >= content.size() || content[pos] != '"')
            {
                return false;
            }

            size_t valueStart = pos;
            ++pos;

            while (pos < content.size() && content[pos] != '"')
            {
                ++pos;
            }

            if (pos >= content.size())
            {
                return false;
            }

            ++pos;

            if (pos >= content.size() || content[pos] != ':')
            {
                return false;
            }

            std::string token = content.substr(valueStart, pos - valueStart);

            if (!parseString(token, key3))
            {
                return false;
            }

            hasKey3 = true;
        }
        else
        {
            return false;
        }
    }

    if (!(hasKey1 && hasKey2 && hasKey3))
    {
        return false;
    }

    result = { key1, key2, key3 };
    return true;
}

std::istream& operator>>(std::istream& in, DataStruct& value)
{
    std::string line;

    if (!std::getline(in, line))
    {
        return in;
    }

    DataStruct temp;

    if (parseRecord(line, temp))
    {
        value = temp;
    }
    else
    {
        in.setstate(std::ios::failbit);
    }

    return in;
}

std::ostream& operator<<(std::ostream& out, const DataStruct& value)
{
    out << "(:key1 " << value.key1 << "ull";
    out << ":key2 0b";

    if (value.key2 == 0)
    {
        out << '0';
    }
    else
    {
        unsigned long long temp = value.key2;
        std::string bin;

        while (temp > 0)
        {
            bin = char('0' + (temp % 2)) + bin;
            temp /= 2;
        }

        out << bin;
    }

    out << ":key3 \"" << value.key3 << "\":)";
    return out;
}

bool compareDataStruct(const DataStruct& lhs, const DataStruct& rhs)
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

int main() {
    std::vector<DataStruct> data;
    std::string line;

    while (std::getline(std::cin, line))
    {
        if (line.empty())
        {
            break;
        }

        std::istringstream lineStream(line);

        std::copy(
            std::istream_iterator<DataStruct>(lineStream),
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

