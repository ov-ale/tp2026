#include <iostream>
#include <string>
#include <vector>

struct DataStruct
{
    double key1;
    unsigned long long key2;
    std::string key3;
};

std::string extractValue(const std::string& line, const std::string& key)
{
    std::string search = ":" + key + " ";
    size_t pos = line.find(search);

    if (pos == std::string::npos)
    {
        return "";
    }

    pos += search.length();
    size_t end = line.find(':', pos);

    if (end == std::string::npos)
    {
        return "";
    }
    return line.substr(pos, end - pos);
}


int main()
{


    return 0;
}
