#include <iostream>
#include <string>
#include <vector>
#include <iterator>
#include <algorithm>
#include <iomanip>
#include <limits>

struct DataStruct {
    double key1;
    long long key2;
    std::string key3;
};

struct DelimiterIO {
    char expected;
} ;

std::istream& operator>>(std::istream& in, DelimiterIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) {
        return in;
    }

    char c = '0';
    in >> c;

    if (in && c != dest.expected) {
        in.setstate(std::ios::failbit);
    }

    return in;
}


struct StringIO {
    std::string& ref;
} ;

std::istream& operator>>(std::istream& in, StringIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) {
        return in;
    }

    in >> DelimiterIO{'"'};
    if (!in) {
        return in;
    }

    std::getline(in, dest.ref, '"');

    return in;
}


struct DoubleSciIO {
    double& ref;
} ;

std::istream& operator>>(std::istream& in, DoubleSciIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) {
        return in;
    }

    std::streampos pos = in.tellg();

    std::string num;
    in >> num;

    if (!in) {
        return in;
    }

    bool valid = false;

    size_t dotPos = num.find('.');
    if (dotPos != std::string::npos) {
        bool hasDigitBefore = dotPos > 0 && std::isdigit(num[dotPos - 1]);
        bool hasDigitAfter = dotPos + 1 < num.length() && std::isdigit(num[dotPos + 1]);

        if (hasDigitBefore && hasDigitAfter) {
            size_t expPos = num.find_first_of("eE");
            if (expPos != std::string::npos && expPos > dotPos) {
                char sign = num[expPos + 1];
                size_t startExp = expPos + 1;
                if (sign == '+' || sign == '-') {
                    startExp++;
                }

                if (startExp < num.length() && std::isdigit(num[startExp])) {
                    valid = true;
                }
            }
        }

    }

    if (valid) {
        try {
            dest.ref = std::stod(num);
        }
        catch (...) {
            valid = false;
        }
    }

    if (!valid) {
        in.clear();
        in.seekg(pos);
        in.setstate(std::ios::failbit);
    }

    return in;
}


struct LongLongIO {
    long long& ref;
} ;

std::istream& operator>>(std::istream& in, LongLongIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) {
        return in;
    }

    std::streampos pos = in.tellg();

    in >> dest.ref;
    if (!in) {
        return in;
    }

    char first = in.get();
    char second = in.get();

    bool isValid = (first == 'l' || first == 'L') && (second == 'l' || second == 'L');

    if (!isValid) {
        in.clear();
        in.seekg(pos);
        in.setstate(std::ios::failbit);
    }

    return in;
}


std::istream& operator>>(std::istream& in, DataStruct& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) {
        return in;
    }

    DataStruct temp;
    bool key1_read = false;
    bool key2_read = false;
    bool key3_read = false;

    in >> DelimiterIO{'('};
    if (!in) {
        return in;
    }

    for (int i = 0; i < 3; ++i) {
        in >> DelimiterIO{':'};
        if (!in) {
            return in;
        }

        std::string label;
        in >> label;
        if (!in) {
            return in;
        }

        in >> DelimiterIO{' '};
        if (!in) {
            return in;
        }

        if (label == "key1" && !key1_read) {
            in >> DoubleSciIO{temp.key1};
            key1_read = true;
        }
        else if (label == "key2" && !key2_read) {
            in >> LongLongIO{temp.key2};
            key2_read = true;
        }
        else if (label == "key3" && !key3_read) {
            in >> StringIO{temp.key3};
            key3_read = true;
        }
        else {
            in.setstate(std::ios::failbit);
            return in;
        }

        if (!in) {
            return in;
        }
    }

    in >> DelimiterIO{':'} >> DelimiterIO{')'};

    if (in && key1_read && key2_read && key3_read) {
        dest = std::move(temp);
    }
    else {
        in.setstate(std::ios::failbit);
    }

    return in;
}


class iofmtguard {
private:
    std::basic_ios<char>& s_;
    std::streamsize width_;
    char fill_;
    std::streamsize precision_;
    std::ios::fmtflags fmt_;
public:
    iofmtguard(std::basic_ios<char>& s) : s_(s) {
        width_ = s.width();
        fill_ = s.fill();
        precision_ = s.precision();
        fmt_ = s.flags();
    }

    ~iofmtguard() {
        s_.width(width_);
        s_.fill(fill_);
        s_.precision(precision_);
        s_.flags(fmt_);
    }
} ;


std::ostream& operator<<(std::ostream& out, const DataStruct& source) {
    std::ostream::sentry sentry(out);
    if (!sentry) {
        return out;
    }

    iofmtguard guard(out);

    out << "(:key1 ";
    out << std::scientific << std::setprecision(1) << source.key1;

    out << ":key2 " << source.key2 << "ll";
    out << ":key3 \"" << source.key3 << "\":)";

    return out;
}


bool compareDataStruct(const DataStruct& a, const DataStruct& b) {
    if (std::abs(a.key1 - b.key1) > 1e-9) {
        return a.key1 < b.key1;
    }
    if (a.key2 != b.key2) {
        return a.key2 < b.key2;
    }
    return a.key3.length() < b.key3.length();
}


int main(void) {
    std::vector<DataStruct> data;

    std::copy(
        std::istream_iterator<DataStruct>(std::cin),
        std::istream_iterator<DataStruct>(),
        std::back_inserter(data)
    );

    if (std::cin.fail() && !std::cin.eof()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    std::sort(data.begin(), data.end(), compareDataStruct);

    std::copy(
        data.begin(),
        data.end(),
        std::ostream_iterator<DataStruct>(std::cout, "\n")
    );

    return 0;
}
