#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include <limits>
#include <utility>

struct DataStruct {
    char key1 = '\0';
    std::pair<long long, unsigned long long> key2 = {0, 0};
    std::string key3;
};

void skipToRecordEnd(std::istream& is) {
    char ch;
    while (is >> ch && ch != ')') {}
}

std::istream& operator>>(std::istream& is, DataStruct& obj) {
    obj.key1 = '\0';
    obj.key2 = {0, 0};
    obj.key3.clear();

    char ch;
    if (!(is >> ch) || ch != '(') {
        return is;
    }

    bool has_key1 = false, has_key2 = false, has_key3 = false;
    bool valid = true;

    while (is >> ch && ch != ')') {
        if (ch == ':') {
            std::string field;
            if (!(is >> field)) {
                skipToRecordEnd(is);
                return is;
            }

            if (field == "key1") {
                char q1, val, q2;
                if (!(is >> q1 >> val >> q2) || q1 != '\'' || q2 != '\'') {
                    valid = false;
                    skipToRecordEnd(is);
                    return is;
                }
                if (!(is >> ch) || ch != ':') {
                    valid = false;
                    skipToRecordEnd(is);
                    return is;
                }
                obj.key1 = val;
                has_key1 = true;
            }
            else if (field == "key2") {
                char open_p;
                if (!(is >> open_p) || open_p != '(') {
                    valid = false;
                    skipToRecordEnd(is);
                    return is;
                }
                std::string tag_n;
                if (!(is >> tag_n) || tag_n != ":N") {
                    valid = false;
                    skipToRecordEnd(is);
                    return is;
                }
                long long n;
                if (!(is >> n)) {
                    valid = false;
                    skipToRecordEnd(is);
                    return is;
                }
                std::string tag_d;
                if (!(is >> tag_d) || tag_d != ":D") {
                    valid = false;
                    skipToRecordEnd(is);
                    return is;
                }
                unsigned long long d;
                    if (!(is >> d) || d==0) {
                    valid = false;
                     skipToRecordEnd(is);
                    return is;
                }
                char c1, c2;
                if (!(is >> c1 >> c2) || c1 != ':' || c2 != ')') {
                    valid = false;
                    skipToRecordEnd(is);
                    return is;
                }
                if (!(is >> ch) || ch != ':') {
                    valid = false;
                    skipToRecordEnd(is);
                    return is;
                }
                obj.key2 = {n, d};
                has_key2 = true;
            }
            else if (field == "key3") {
                char quote;
                if (!(is >> quote) || quote != '"') {
                    valid = false;
                    skipToRecordEnd(is);
                    return is;
                }
                if (!std::getline(is, obj.key3, '"')) {
                    valid = false;
                    skipToRecordEnd(is);
                    return is;
                }
                if (!(is >> ch) || ch != ':') {
                    valid = false;
                    skipToRecordEnd(is);
                    return is;
                }
                has_key3 = true;
            }
            else {
                skipToRecordEnd(is);
                return is;
            }
        }
    }

    if (!valid || !has_key1 || !has_key2 || !has_key3) {
        obj.key1 = '\0';
    }

    return is;
}

std::ostream& operator<<(std::ostream& os, const DataStruct& obj) {
    os << "(:key1 '" << obj.key1 << "':";
    os << "key2 (:N " << obj.key2.first << ":D " << obj.key2.second << ":):";
    os << "key3 \"" << obj.key3 << "\":)";
    return os;
}

struct Comparator {
    bool operator()(const DataStruct& a, const DataStruct& b) const {
        if (a.key1 != b.key1) return a.key1 < b.key1;
        double va = static_cast<double>(a.key2.first) / a.key2.second;
        double vb = static_cast<double>(b.key2.first) / b.key2.second;
        if (va != vb) return va < vb;
        return a.key3.length() < b.key3.length();
    }
};

int main() {
    std::vector<DataStruct> data;
    while (!std::cin.eof())
    {
        std::copy(
            std::istream_iterator<DataStruct>(std::cin),
            std::istream_iterator<DataStruct>(),
            std::back_inserter(data)
        );
        if (!std::cin.eof()){
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    if (data.empty()) {
        std::cout<< "Atleast one supported record type" << std::endl;
        std::sort(data.begin(), data.end(), Comparator());
        std::copy(
            data.begin(),
            data.end(),
            std::ostream_iterator<DataStruct>(std::cout, "\n")
        );
        return 0;
    }
    return 0;
}
