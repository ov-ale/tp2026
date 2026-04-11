#include "DataStruct.h"
#include <iomanip>
#include <algorithm>
std::istream& operator>>(std::istream& in, DelimiterIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) {
        return in;
    }
    char c=' ';
    in>>c;
    if (in&&std::tolower(c)!=std::tolower(dest.exp)) {
        in.setstate(std::ios::failbit);
    }
    return in;
}
std::istream& operator>>(std::istream& in, LabelIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) {
        return in;
    }
    for (char exp_c:dest.exp) {
        in>>DelimiterIO{exp_c};
    }
    return in;
}
std::istream& operator>>(std::istream& in, DoubleLitIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) {
        return in;
    }
    double val=0.0;
    in>>val>>DelimiterIO{'d'};
    if (in) {
        dest.ref=val;
    }
    return in;
}
std::istream& operator>>(std::istream& in, UllLitIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) {
        return in;
    }
    unsigned long long val=0;
    in>>val>>LabelIO{"ull"};
    if (in) {
        dest.ref=val;
    }
    return in;
}

std::istream& operator>>(std::istream& in, StringIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) {
        return in;
    }
    return std::getline(in>>DelimiterIO{'"'}, dest.ref, '"');
}
std::istream& operator>>(std::istream& in, DataStruct& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) {
        return in;
    }
    DataStruct temp{0.0, 0, ""};
    in>>DelimiterIO{'('};
    for (int i=0; i<3; ++i) {
        std::string label;
        in>>DelimiterIO{':'}; 
        if (!(in>>label)) {
            break;
        }
        if (label=="key1") {
            in>>DoubleLitIO{temp.key1};
        }
        else if (label=="key2") {
            in>>UllLitIO{temp.key2};
        }
        else if (label=="key3") {
            in>>StringIO{temp.key3};
        }
        else {
            in.setstate(std::ios::failbit);
        }
    }
    in>>DelimiterIO{':'}>>DelimiterIO{')'};
    if (in) {
        dest=temp;
    }
    return in;
}
std::ostream& operator<<(std::ostream& out, const DataStruct& src) {
    std::ostream::sentry sentry(out);
    if (!sentry) {
        return out;
    }
    out<<std::fixed<<std::setprecision(1);
    out<<"(:key1 "<<src.key1<<"d:key2 "<<src.key2<<"ull:key3 \""<<src.key3<<"\":)";
    return out;
}
bool compareData(const DataStruct& lhs, const DataStruct& rhs) {
    if (lhs.key1!=rhs.key1) {
        return lhs.key1<rhs.key1;
    }
    if (lhs.key2!=rhs.key2) {
        return lhs.key2<rhs.key2;
    }
    return lhs.key3.length()<rhs.key3.length();
}