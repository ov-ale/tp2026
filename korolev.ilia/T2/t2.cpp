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
    int depth=1;
    while (depth>0 && is>>ch ){
        if (ch== '('){
            depth++;
        } else if (ch == ')'){
            depth--;
        }
    }
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
            is >>std::ws;
            if (is.peek()==')'){
                continue;
            }
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
                    break;
                }
                obj.key1 = val;
                has_key1 = true;
            }
            else if (field == "key2") {
                char open_p;
                if (!(is >> open_p) || open_p != '(') {
                    valid = false;
                    skipToRecordEnd(is);
                    break;
                }
                char c1, c2;
                if (!(is >> c1 >> c2) || c1 != ':' || c2!= 'N') {
                    valid = false;
                    skipToRecordEnd(is);
                    break;
                }
                long long n;
                if (!(is >> n)) {
                    valid = false;
                    skipToRecordEnd(is);
                    break;
                }
                if (!(is >> c1 >> c2) || c1!= ':' || c2!='D') {
                    valid = false;
                    skipToRecordEnd(is);
                    break;
                }
                unsigned long long d;
                    if (!(is >> d) || d==0) {
                    valid = false;
                     skipToRecordEnd(is);
                    break;
                }
                if (!(is >> c1 >> c2) || c1 != ':' || c2 != ')') {
                    valid = false;
                    skipToRecordEnd(is);
                    break;
                }
                obj.key2 = {n, d};
                has_key2 = true;
            }
            else if (field == "key3") {
                char quote;
                if (!(is >> quote) || quote != '"') {
                    valid = false;
                    skipToRecordEnd(is);
                    break;
                }
                if (!std::getline(is, obj.key3, '"')) {
                    valid = false;
                    skipToRecordEnd(is);
                    break;
                }
                has_key3 = true;
            }
            else {
                valid=false;
                skipToRecordEnd(is);
                break;
            }
        } else{
            valid = false;
            skipToRecordEnd(is);
            break;
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
    std::copy(
        std::istream_iterator<DataStruct>(std::cin),
        std::istream_iterator<DataStruct>(),
        std::back_inserter(data)
    );
    data.erase(
        std::remove_if(data.begin(), data.end(),
            [](const DataStruct& ds) { return ds.key1 == '\0'; }),
        data.end()
    );
    if (data.empty()){
    }else{
        std::sort(data.begin(), data.end(), Comparator());
        std::copy(
            data.begin(),
            data.end(),
            std::ostream_iterator<DataStruct>(std::cout, "\n")
        );
    }
    return 0;
}
