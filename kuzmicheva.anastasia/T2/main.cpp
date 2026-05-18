#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include <complex>
#include <iomanip>
#include <limits>

namespace nspace {
    struct DataStruct {
        unsigned long long key1;
        std::complex<double> key2;
        std::string key3;
    };

    struct DelimiterIO {
        char exp;
    };
    struct ULLitIO {
        unsigned long long& ref;
    };
    struct ComplexLspIO {
        std::complex<double>& ref;
    };
    struct StringIO {
        std::string& ref;
    };
    struct LabelIO {
        std::string exp;
    };

    class iofmtguard {
    public:
        iofmtguard(std::basic_ios<char>& s);
        ~iofmtguard();
    private:
        std::basic_ios<char>& s_;
        std::streamsize width_;
        char fill_;
        std::streamsize precision_;
        std::basic_ios<char>::fmtflags fmt_;
    };

    std::istream& operator>>(std::istream& in, DelimiterIO&& dest);
    std::istream& operator>>(std::istream& in, ULLitIO&& dest);
    std::istream& operator>>(std::istream& in, ComplexLspIO&& dest);
    std::istream& operator>>(std::istream& in, StringIO&& dest);
    std::istream& operator>>(std::istream& in, LabelIO&& dest);
    std::istream& operator>>(std::istream& in, DataStruct& dest);
    std::ostream& operator<<(std::ostream& out, const DataStruct& src);
}

int main()
{
    using namespace nspace;
    std::vector<DataStruct> data;

    while (!std::cin.eof())
    {
        std::copy(
            std::istream_iterator<DataStruct>(std::cin),
            std::istream_iterator<DataStruct>(),
            std::back_inserter(data)
        );
        if (std::cin.fail() && !std::cin.eof())
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    std::sort(data.begin(), data.end(), [](const DataStruct& a, const DataStruct& b) {
        if (a.key1 != b.key1) {
            return a.key1 < b.key1;
        }
        if (std::abs(a.key2) != std::abs(b.key2)) {
            return std::abs(a.key2) < std::abs(b.key2);
        }
        return a.key3.length() < b.key3.length();
        });

    std::copy(data.begin(), data.end(), std::ostream_iterator<DataStruct>(std::cout, "\n"));

    return 0;
}

namespace nspace
{
    std::istream& operator>>(std::istream& in, DelimiterIO&& dest)
    {
        std::istream::sentry sentry(in);
        if (!sentry) {
            return in;
        }
        char c = '0';
        in >> c;
        if (in && (std::tolower(c) != std::tolower(dest.exp))) {
            in.setstate(std::ios::failbit);
        }
        return in;
    }

    std::istream& operator>>(std::istream& in, ULLitIO&& dest)
    {
        std::istream::sentry sentry(in);
        if (!sentry) {
            return in;
        }
        return in >> dest.ref >> DelimiterIO{ 'u' } >>
            DelimiterIO{ 'l' } >> DelimiterIO{ 'l' };
    }

    std::istream& operator>>(std::istream& in, ComplexLspIO&& dest)
    {
        std::istream::sentry sentry(in);
        if (!sentry) {
            return in;
        }
        double re = 0.0, im = 0.0;
        in >> DelimiterIO{ '#' } >> DelimiterIO{ 'c' } >> DelimiterIO{ '(' } >>
            re >> im >> DelimiterIO{ ')' };
        if (in) {
            dest.ref = { re, im };
        }
        return in;
    }

    std::istream& operator>>(std::istream& in, StringIO&& dest)
    {
        std::istream::sentry sentry(in);
        if (!sentry) {
            return in;
        }
        return in >> std::quoted(dest.ref);
    }

    std::istream& operator>>(std::istream& in, LabelIO&& dest)
    {
        std::istream::sentry sentry(in);
        if (!sentry) {
            return in;
        }
        for (char c : dest.exp) {
            in >> DelimiterIO{ c };
        }
        return in;
    }

    std::istream& operator>>(std::istream& in, DataStruct& dest)
    {
        std::istream::sentry sentry(in);
        if (!sentry) {
            return in;
        }
        DataStruct input;
        using sep = DelimiterIO;
        using label = LabelIO;

        in >> sep{ '(' } >> sep{ ':' };
        for (size_t i = 0; i < 3; ++i) {
            std::string key = "";
            in >> label{ "key" };
            char num;
            in >> num;
            if (num == '1') {
                in >> ULLitIO{ input.key1 };
            }
            else if (num == '2') {
                in >> ComplexLspIO{ input.key2 };
            }
            else if (num == '3') {
                in >> StringIO{ input.key3 };
            }
            else {
                in.setstate(std::ios::failbit);
            }
            in >> sep{ ':' };
        }
        in >> sep{ ')' };

        if (in) {
            dest = std::move(input);
        }
        return in;
    }

    std::ostream& operator<<(std::ostream& out, const DataStruct& src)
    {
        std::ostream::sentry sentry(out);
        if (!sentry) {
            return out;
        }
        iofmtguard fmtguard(out);
        out << "(:key1 " << src.key1 << "ull"
            << ":key2 #c(" << std::fixed << std::setprecision(1) <<
            src.key2.real() << " " << src.key2.imag() << ")"
            << ":key3 " << std::quoted(src.key3) << ":)";
        return out;
    }

    iofmtguard::iofmtguard(std::basic_ios< char >& s) :
        s_(s),
        width_(s.width()),
        fill_(s.fill()),
        precision_(s.precision()),
        fmt_(s.flags())
    {}
    iofmtguard::~iofmtguard()
    {
        s_.width(width_);
        s_.fill(fill_);
        s_.precision(precision_);
        s_.flags(fmt_);
    }
}

