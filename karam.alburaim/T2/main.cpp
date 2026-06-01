#include <iostream>
#include <vector>
#include <string>
#include <complex>
#include <algorithm>
#include <iterator>
#include <iomanip>
#include <limits>
#include <cmath>

namespace lab2 {

    struct DataStruct {
        unsigned long long key1;
        std::complex<double> key2;
        std::string key3;
    };

    struct CharGuard {
        char expected;
    };

    std::istream& operator>>(std::istream& is, CharGuard guard) {
        std::istream::sentry sentry(is);
        if (!sentry) return is;

        char c;
        if (is >> c) {
            if (std::tolower(c) != std::tolower(guard.expected)) {
                is.setstate(std::ios::failbit);
            }
        }
        return is;
    }

    struct ULLSuffixGuard {};

    std::istream& operator>>(std::istream& is, ULLSuffixGuard) {
        std::istream::sentry sentry(is);
        if (!sentry) return is;

        char u = is.get();
        char l1 = is.get();
        char l2 = is.get();
        if (std::tolower(u) != 'u' || std::tolower(l1) != 'l' || std::tolower(l2) != 'l') {
            is.setstate(std::ios::failbit);
        }
        return is;
    }

    class StreamStateSaver {
        std::ostream& os_;
        std::ios_base::fmtflags flags_;
        std::streamsize prec_;
    public:
        StreamStateSaver(std::ostream& os)
            : os_(os), flags_(os.flags()), prec_(os.precision()) {}
        ~StreamStateSaver() {
            os_.flags(flags_);
            os_.precision(prec_);
        }
    };

    struct DataComparator {
        bool operator()(const DataStruct& left, const DataStruct& right) const {
            if (left.key1 != right.key1) {
                return left.key1 < right.key1;
            }
            double absLeft = std::abs(left.key2);
            double absRight = std::abs(right.key2);
            if (std::abs(absLeft - absRight) > 1e-9) {
                return absLeft < absRight;
            }
            return left.key3.length() < right.key3.length();
        }
    };

    std::istream& operator>>(std::istream& is, DataStruct& dest) {
        std::istream::sentry sentry(is);
        if (!sentry) return is;

        DataStruct temp;
        int keysMask = 0;

        if (!(is >> CharGuard{'('} >> CharGuard{':'})) return is;

        for (int i = 0; i < 3; ++i) {
            std::string keyName;
            is >> keyName;

            if (keyName == "key1" && !(keysMask & 1)) {
                is >> temp.key1 >> ULLSuffixGuard{};
                keysMask |= 1;
            }
            else if (keyName == "key2" && !(keysMask & 2)) {
                double realVal, imagVal;
                is >> CharGuard{'#'} >> CharGuard{'c'} >> CharGuard{'('}
                   >> realVal >> imagVal >> CharGuard{')'};
                if (is) temp.key2 = std::complex<double>(realVal, imagVal);
                keysMask |= 2;
            }
            else if (keyName == "key3" && !(keysMask & 4)) {
                is >> std::quoted(temp.key3);
                keysMask |= 4;
            }
            else {
                is.setstate(std::ios::failbit);
                return is;
            }
            is >> CharGuard{':'};
        }

        is >> CharGuard{')'};

        if (is && keysMask == 7) {
            dest = std::move(temp);
        } else {
            is.setstate(std::ios::failbit);
        }

        return is;
    }

    std::ostream& operator<<(std::ostream& os, const DataStruct& src) {
        std::ostream::sentry sentry(os);
        if (!sentry) return os;

        StreamStateSaver saver(os);

        os << "(:key1 " << src.key1 << "ull"
           << ":key2 #c(" << std::fixed << std::setprecision(1)
           << src.key2.real() << " " << src.key2.imag() << ")"
           << ":key3 " << std::quoted(src.key3) << ":)";

        return os;
    }
}

int main() {
    std::vector<lab2::DataStruct> recordsList;

    while (!std::cin.eof()) {
        std::copy(
            std::istream_iterator<lab2::DataStruct>(std::cin),
            std::istream_iterator<lab2::DataStruct>(),
            std::back_inserter(recordsList)
        );

        if (std::cin.fail() && !std::cin.eof()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    std::sort(recordsList.begin(), recordsList.end(), lab2::DataComparator());

    std::copy(
        recordsList.begin(),
        recordsList.end(),
        std::ostream_iterator<lab2::DataStruct>(std::cout, "\n")
    );

    return 0;
}
#include <iostream>
#include <vector>
#include <string>
#include <complex>
#include <algorithm>
#include <iterator>
#include <iomanip>
#include <limits>
#include <cmath>

namespace lab2 {

    struct DataStruct {
        unsigned long long key1;
        std::complex<double> key2;
        std::string key3;
    };

    struct CharGuard {
        char expected;
    };

    std::istream& operator>>(std::istream& is, CharGuard guard) {
        std::istream::sentry sentry(is);
        if (!sentry) return is;

        char c;
        if (is >> c) {
            if (std::tolower(c) != std::tolower(guard.expected)) {
                is.setstate(std::ios::failbit);
            }
        }
        return is;
    }

    struct ULLSuffixGuard {};

    std::istream& operator>>(std::istream& is, ULLSuffixGuard) {
        std::istream::sentry sentry(is);
        if (!sentry) return is;

        char u = is.get();
        char l1 = is.get();
        char l2 = is.get();
        if (std::tolower(u) != 'u' || std::tolower(l1) != 'l' || std::tolower(l2) != 'l') {
            is.setstate(std::ios::failbit);
        }
        return is;
    }

    class StreamStateSaver {
        std::ostream& os_;
        std::ios_base::fmtflags flags_;
        std::streamsize prec_;
    public:
        StreamStateSaver(std::ostream& os)
            : os_(os), flags_(os.flags()), prec_(os.precision()) {}
        ~StreamStateSaver() {
            os_.flags(flags_);
            os_.precision(prec_);
        }
    };

    struct DataComparator {
        bool operator()(const DataStruct& left, const DataStruct& right) const {
            if (left.key1 != right.key1) {
                return left.key1 < right.key1;
            }
            double absLeft = std::abs(left.key2);
            double absRight = std::abs(right.key2);
            if (std::abs(absLeft - absRight) > 1e-9) {
                return absLeft < absRight;
            }
            return left.key3.length() < right.key3.length();
        }
    };

    std::istream& operator>>(std::istream& is, DataStruct& dest) {
        std::istream::sentry sentry(is);
        if (!sentry) return is;

        DataStruct temp;
        int keysMask = 0;

        if (!(is >> CharGuard{'('} >> CharGuard{':'})) return is;

        for (int i = 0; i < 3; ++i) {
            std::string keyName;
            is >> keyName;

            if (keyName == "key1" && !(keysMask & 1)) {
                is >> temp.key1 >> ULLSuffixGuard{};
                keysMask |= 1;
            }
            else if (keyName == "key2" && !(keysMask & 2)) {
                double realVal, imagVal;
                is >> CharGuard{'#'} >> CharGuard{'c'} >> CharGuard{'('}
                   >> realVal >> imagVal >> CharGuard{')'};
                if (is) temp.key2 = std::complex<double>(realVal, imagVal);
                keysMask |= 2;
            }
            else if (keyName == "key3" && !(keysMask & 4)) {
                is >> std::quoted(temp.key3);
                keysMask |= 4;
            }
            else {
                is.setstate(std::ios::failbit);
                return is;
            }
            is >> CharGuard{':'};
        }

        is >> CharGuard{')'};

        if (is && keysMask == 7) {
            dest = std::move(temp);
        } else {
            is.setstate(std::ios::failbit);
        }

        return is;
    }

    std::ostream& operator<<(std::ostream& os, const DataStruct& src) {
        std::ostream::sentry sentry(os);
        if (!sentry) return os;

        StreamStateSaver saver(os);

        os << "(:key1 " << src.key1 << "ull"
           << ":key2 #c(" << std::fixed << std::setprecision(1)
           << src.key2.real() << " " << src.key2.imag() << ")"
           << ":key3 " << std::quoted(src.key3) << ":)";

        return os;
    }
}

int main() {
    std::vector<lab2::DataStruct> recordsList;

    while (!std::cin.eof()) {
        std::copy(
            std::istream_iterator<lab2::DataStruct>(std::cin),
            std::istream_iterator<lab2::DataStruct>(),
            std::back_inserter(recordsList)
        );

        if (std::cin.fail() && !std::cin.eof()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    std::sort(recordsList.begin(), recordsList.end(), lab2::DataComparator());

    std::copy(
        recordsList.begin(),
        recordsList.end(),
        std::ostream_iterator<lab2::DataStruct>(std::cout, "\n")
    );

    return 0;
}

