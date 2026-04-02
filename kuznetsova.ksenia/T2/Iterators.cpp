#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <iomanip>
#include <cmath>
#include <limits>
#include <complex>

namespace nspace {
    const double EPS = 1e-9;

    struct DataStruct {
        unsigned long long key1 = 0;
        std::complex<double> key2{ 0.0, 0.0 };
        std::string key3;
    };

    struct DelimiterIO { char exp; };
    struct KeyIO { int& ref; };

    std::istream& operator>>(std::istream& in, KeyIO&& dest) {
        std::istream::sentry sentry(in);
        if (!sentry) return in;
        char k, e, y, n;
        in >> k >> e >> y >> n;
        if (in && k == 'k' && e == 'e' && y == 'y' && n >= '1' && n <= '3') {
            dest.ref = n - '0';
        }
        else {
            in.setstate(std::ios::failbit);
        }
        return in;
    }

    std::istream& operator>>(std::istream& in, DelimiterIO&& dest) {
        std::istream::sentry sentry(in);
        if (!sentry) return in;
        char ch;
        in >> ch;
        if (in && std::tolower(ch) != std::tolower(dest.exp)) {
            in.setstate(std::ios::failbit);
        }
        return in;
    }

    bool readUllHex(std::istream& in, unsigned long long& val) {
        std::istream::sentry sentry(in);
        if (!sentry) return false;

        char c0, c1;
        in >> c0 >> c1;
        if (c0 != '0' || (c1 != 'x' && c1 != 'X')) {
            in.setstate(std::ios::failbit);
            return false;
        }

        if (!(in >> std::hex >> val)) {
            in.setstate(std::ios::failbit);
            return false;
        }
        in >> std::dec;

        char colon;
        return (in >> colon) && (colon == ':');
    }

    bool readCmpLsp(std::istream& in, std::complex<double>& val) {
        std::istream::sentry sentry(in);
        if (!sentry) return false;

        char hash, c, open, close, colon;
        double real, imag;

        in >> hash >> c >> open;
        if (hash != '#' || (c != 'c' && c != 'C') || open != '(') {
            in.setstate(std::ios::failbit);
            return false;
        }

        if (!(in >> real >> imag)) {
            in.setstate(std::ios::failbit);
            return false;
        }

        in >> close >> colon;
        if (close != ')' || colon != ':') {
            in.setstate(std::ios::failbit);
            return false;
        }

        val = std::complex<double>(real, imag);
        return true;
    }

    std::istream& operator>>(std::istream& in, DataStruct& dest) {
        std::istream::sentry sentry(in);
        if (!sentry) return in;

        DataStruct input;

        if (!(in >> DelimiterIO{ '(' } >> DelimiterIO{ ':' })) {
            return in;
        }

        bool has[3] = { false, false, false };

        for (int i = 0; i < 3 && in; i++) {
            int keyNum = 0;
            in >> KeyIO{ keyNum };
            if (!in) break;

            in >> DelimiterIO{ ':' };
            if (!in) break;

            if (keyNum == 1 && !has[0]) {
                has[0] = readUllHex(in, input.key1);
            }
            else if (keyNum == 2 && !has[1]) {
                has[1] = readCmpLsp(in, input.key2);
            }
            else if (keyNum == 3 && !has[2]) {
                char quote;
                in >> quote;
                if (quote == '"') {
                    std::getline(in, input.key3, '"');
                    in >> DelimiterIO{ ':' };
                    has[2] = true;
                }
                else {
                    in.setstate(std::ios::failbit);
                }
            }
            else {
                in.setstate(std::ios::failbit);
                break;
            }
        }

        if (in) {
            in >> DelimiterIO{ ')' };
        }

        if (in && has[0] && has[1] && has[2]) {
            dest = input;
        }
        else {
            in.setstate(std::ios::failbit);
        }
        return in;
    }

    std::ostream& operator<<(std::ostream& out, const DataStruct& src) {
        std::ostream::sentry sentry(out);
        if (!sentry) return out;

        out << "(:key1 0x" << std::hex << std::uppercase << src.key1
            << std::dec << std::nouppercase;
        out << ":key2 #c(" << std::fixed << std::setprecision(1)
            << src.key2.real() << " " << src.key2.imag() << ")";
        out << ":key3 \"" << src.key3 << "\":)";

        return out;
    }

    bool compareData(const DataStruct& a, const DataStruct& b) {
        if (a.key1 != b.key1) {
            return a.key1 < b.key1;
        }

        double modA = std::abs(a.key2);
        double modB = std::abs(b.key2);
        if (std::abs(modA - modB) > EPS) {
            return modA < modB;
        }

        return a.key3.length() < b.key3.length();
    }
}


int main() {
    std::vector<nspace::DataStruct> data;

    while (std::cin) {
        std::copy(
            std::istream_iterator<nspace::DataStruct>(std::cin),
            std::istream_iterator<nspace::DataStruct>(),
            std::back_inserter(data)
        );

        if (std::cin.fail() && !std::cin.eof()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        else if (std::cin.eof()) {
            break;
        }
    }

    if (data.empty()) {
        std::cerr << "Looks like there is no supported record. Cannot determine input. Test skipped" << std::endl;
        return 0;
    }

    std::sort(data.begin(), data.end(), nspace::compareData);

    std::copy(
        data.begin(),
        data.end(),
        std::ostream_iterator<nspace::DataStruct>(std::cout, "\n")
    );

    return 0;
}
