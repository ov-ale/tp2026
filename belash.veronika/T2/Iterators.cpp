#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <iomanip>
#include <cctype>

struct DataStruct {
    double key1;
    unsigned long long key2;
    std::string key3;
};

class iofmtguard {
public:
    explicit iofmtguard(std::basic_ios<char>& s) :
        s_(s), width_(s.width()), precision_(s.precision()), fill_(s.fill()), fmt_(s.flags()) {
    }
    ~iofmtguard() {
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

std::string extractValue(const std::string& line, const std::string& key) {
    std::string search = ":" + key + " ";
    size_t pos = line.find(search);
    if (pos == std::string::npos) return "";
    pos += search.length();
    size_t end = line.find(':', pos);
    if (end == std::string::npos) return "";
    return line.substr(pos, end - pos);
}

std::istream& operator>>(std::istream& in, DataStruct& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) return in;
    std::string line;
    while (std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.size() < 4 || line[0] != '(' || line[1] != ':' ||
            line[line.size() - 2] != ':' || line[line.size() - 1] != ')') continue;

        std::string v1 = extractValue(line, "key1");
        std::string v2 = extractValue(line, "key2");
        std::string v3 = extractValue(line, "key3");

        if (v1.empty() || v2.empty() || v3.empty()) continue;

        std::istringstream iss1(v1);
        double val1; char s1;
        if (!(iss1 >> val1 >> s1)) continue;
        if (s1 != 'd' && s1 != 'D') continue;
        char extra1;
        if (iss1 >> extra1) continue;

        std::istringstream iss2(v2);
        unsigned long long val2; std::string s2;
        if (!(iss2 >> val2 >> s2)) continue;
        for (char& c : s2) c = std::tolower(static_cast<unsigned char>(c));
        if (s2 != "ull") continue;
        char extra2;
        if (iss2 >> extra2) continue;

        if (v3.size() < 2 || v3.front() != '"' || v3.back() != '"') continue;

        dest.key1 = val1;
        dest.key2 = val2;
        dest.key3 = v3.substr(1, v3.size() - 2);
        return in;
    }
    in.setstate(std::ios::failbit);
    return in;
}

std::ostream& operator<<(std::ostream& out, const DataStruct& src) {
    std::ostream::sentry sentry(out);
    if (!sentry) return out;
    iofmtguard fmtguard(out);
    out << "(:key1 " << std::fixed << std::setprecision(1) << src.key1 << "d:key2 "
        << src.key2 << "ull:key3 \"" << src.key3 << "\":)";
    return out;
}

bool compareDataStruct(const DataStruct& a, const DataStruct& b) {
    if (a.key1 != b.key1) return a.key1 < b.key1;
    if (a.key2 != b.key2) return a.key2 < b.key2;
    return a.key3.length() < b.key3.length();
}

int main() {
    std::vector<DataStruct> vec;
    std::copy(std::istream_iterator<DataStruct>(std::cin),
        std::istream_iterator<DataStruct>(),
        std::back_inserter(vec));
    std::sort(vec.begin(), vec.end(), compareDataStruct);
    std::copy(vec.begin(), vec.end(),
        std::ostream_iterator<DataStruct>(std::cout, "\n"));
    return 0;
}
