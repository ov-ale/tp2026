#include "datastruct.h"

bool compareDataStruct(const DataStruct& firstData, const DataStruct& secondData){
    if(firstData.key1 != secondData.key1) return firstData.key1 < secondData.key1;
    if(firstData.key2 != secondData.key2) return firstData.key2 < secondData.key2;
    return firstData.key3.length() < secondData.key3.length();
}

std::string toBinary(unsigned long long value){
    if(value == 0) return "0";

    std::string result;
    while(value > 0){
        result = (value & 1 ? '1' : '0') + result;
        value>>=1;
    }
    return result;
}

std::istream& operator>>(std::istream& in, DelimiterIO&& dest){
    std::istream::sentry sentry(in);
    if(!sentry) return in;
    char c = '\0';
    in >> c;
    if(in && (c != dest.exp)){
        in.setstate(std::ios::failbit);
    }
    return in;
}

std::istream& operator>>(std::istream& in, UllLitIO&& dest){
    std::istream::sentry sentry(in);
    if(!sentry){
        std::cout<<"sentry broke?\n";
        return in;
    }

    std::string token;
    char c;
    while(in.get(c) && c != ':' && c != ' ' && c != '\t'){
        token +=c;
    }
    if(in && (c == ':' || c == ' ' || c == '\t')){
        in.putback(c);
    }
    if (!in) return in;

    size_t pos = 0;
    unsigned long long value = 0;

    try{
        value = std::stoull(token, &pos);
    }
    catch(const std::exception&){
        in.setstate(std::ios::failbit);
        return in;
    }

    if(pos >= token.length()){
        in.setstate(std::ios::failbit);
        return in;
    }

    std::string suffix = token.substr(pos);

    for(size_t i = 0; i < suffix.length(); ++i){
        suffix[i] = std::tolower(suffix[i]);
    }
    if(suffix == "ull"){
        dest.ref = value;
    }
    else{
        in.setstate(std::ios::failbit);
    }
    return in;
}

std::istream& operator>>(std::istream& in, UllBinIO&& dest){
    std::istream::sentry sentry(in);
    if(!sentry){
        return in;
    }

    std::string token;
    char c;
    while(in.get(c) && c != ':' && c != ' ' && c != '\t'){
        token +=c;
    }
    if(in && (c == ':' || c == ' ' || c == '\t')){
        in.putback(c);
    }
    if(!in) return in;

    if(token.length() < 3){
        in.setstate(std::ios::failbit);
        return in;
    }

    if(token[0] !='0' || (token[1] !='b' && token[1] != 'B')){
        in.setstate(std::ios::failbit);
        return in;
    }

    std::string binaryStr = token.substr(2);

    for(char c : binaryStr){
        if(c != '0' && c !='1'){
            in.setstate(std::ios::failbit);
            return in;
        }
    }

    unsigned long long value = 0;

    for(char c : binaryStr){
        value = (value << 1) | (c - '0');
    }

    dest.ref = value;
    return in;
}

std::istream& operator>>(std::istream& in, StringIO&& dest){
    std::istream::sentry sentry(in);
    if(!sentry){
        return in;
    }

    char quote;
    in>>quote;
    if(!in || quote != '"'){
        in.setstate(std::ios::failbit);
        return in;
    }

    std::string value;
    std::getline(in, value, '"');
    if(!in){
        in.setstate(std::ios::failbit);
        return in;
    }

    dest.ref = value;
    return in;
}

std::istream& operator>>(std::istream& in, DataStruct& dest){
    std::istream::sentry sentry(in);
    if(!sentry){
        return in;
    }
    DataStruct temp;
    bool hasKey1 = false;
    bool hasKey2 = false;
    bool hasKey3 = false;

    in >> DelimiterIO{'('} >> DelimiterIO{':'};
    if(!in) return in;

    while(true){
        std::string label;
        in >> label;
        if(!in) return in;

        if(label != "key1" && label != "key2" && label != "key3"){
            in.setstate(std::ios::failbit);
            return in;
        }
        if(in.peek() != ' '){
            in.setstate(std::ios::failbit);
            return in;
        }

        in.ignore();

        if(label == "key1"){
            if(hasKey1){
                in.setstate(std::ios::failbit);
                return in;
            }
            in >> UllLitIO{temp.key1};
            if(!in) return in;
            hasKey1 = true;
        }
        else if(label == "key2"){
            if(hasKey2){
                in.setstate(std::ios::failbit);
                return in;
            }
            in >> UllBinIO{temp.key2};
            if(!in) return in;
            hasKey2 = true;
        }
        else if(label == "key3"){
            if(hasKey3){
                in.setstate(std::ios::failbit);
                return in;
            }
            in >> StringIO{temp.key3};
            if(!in) return in;
            hasKey3 = true;
        }

        char next;
        in >> next;
        if(!in) return in;

        if(next == ':'){
            if(in.peek() == ')'){
                in.ignore();
                break;
            }
        }
        else{
            in.setstate(std::ios::failbit);
            return in;
        }
    }

    if(!hasKey1 || !hasKey2 || !hasKey3){
        in.setstate(std::ios::failbit);
        return in;
    }

    dest = std::move(temp);
    return in;

}

std::ostream& operator<<(std::ostream& out, const DataStruct& dest){
    std::ostream::sentry sentry(out);
    if(!sentry) return out;

    out<<"(:key1 " << dest.key1 << "ull"
    <<":key2 0b" << toBinary(dest.key2)
    <<":key3 \"" << dest.key3 <<"\":)";
    return out;

}

