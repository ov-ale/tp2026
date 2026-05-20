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
    if (in && c!=dest.exp) {
        in.setstate(std::ios::failbit);
    }
    return in;
}
std::istream& operator>>(std::istream& in, DoubleLitIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) {
        return in;
    }
    double val=0.0;
    in>>val;
    if (!in) {
        return in;
    }
    char suffix=' ';
    in>>suffix;
    if (in && (suffix=='d' || suffix=='D')) {
        dest.ref=val;
    } 
    else {
        in.setstate(std::ios::failbit);
    }
    return in;
}
std::istream& operator>>(std::istream& in, UllLitIO&& dest) {
    std::istream::sentry sentry(in);
    if (!sentry) {
        return in;
    }
    unsigned long long val=0;
    in>>val;
    if (!in) {
        return in;
    }
    char u=' ', l1=' ', l2=' ';
    in>>u>>l1>>l2;
    if (in && (u=='u' || u=='U') && (l1=='l' || l1=='L') && (l2=='l' || l2=='L')) {
        dest.ref=val;
    } 
    else {
        in.setstate(std::ios::failbit);
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
    out<<"(:key1 "<<std::fixed<<std::setprecision(1)<<src.key1<<"d";
    out<<":key2 "<<src.key2<<"ull";
    out<<":key3 \""<<src.key3<<"\":)";
    return out;
}
bool compareData(const DataStruct& lhs, const DataStruct& rhs) {
    if (lhs.key1<rhs.key1) {
        return true;
    }
    if (rhs.key1<lhs.key1) {
        return false;
    }
    if (lhs.key2<rhs.key2) {
        return true;
    }
    if (rhs.key2<lhs.key2) {
        return false;
    }
    return lhs.key3.length()<rhs.key3.length();
}