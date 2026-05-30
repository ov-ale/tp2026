#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <limits>
#include <iomanip>
#include <utility>
#include <cmath>

namespace barinov
{
  struct DelimiterIO
  {
    char expected;
  };

  struct LongLongIO
  {
    long long& value;
  };

  struct RationalIO
  {
    std::pair<long long, unsigned long long>& value;
  };

  struct StringIO
  {
    std::string& value;
  };

  std::istream& operator>>(std::istream& in, DelimiterIO&& dest)
  {
    std::istream::sentry sentry(in);
    if (!sentry)
    {
      return in;
    }
    char c = '\0';
    in >> c;
    if (in && c != dest.expected)
    {
      in.setstate(std::ios::failbit);
    }
    return in;
  }

  std::istream& operator>>(std::istream& in, LongLongIO&& dest)
  {
    std::istream::sentry sentry(in);
    if (!sentry)
    {
      return in;
    }
    long long value = 0;
    in >> value;
    if (!in)
    {
      return in;
    }
    char c1 = '\0', c2 = '\0';
    if (in.get(c1) && in.get(c2))
    {
      if ((c1 == 'L' || c1 == 'l') && (c2 == 'L' || c2 == 'l'))
      {
        dest.value = value;
        return in;
      }
    }
    in.setstate(std::ios::failbit);
    return in;
  }

  std::istream& operator>>(std::istream& in, RationalIO&& dest)
  {
    std::istream::sentry sentry(in);
    if (!sentry)
    {
      return in;
    }
    in >> DelimiterIO{ '(' };
    in >> DelimiterIO{ ':' };
    std::string label;
    in >> label;
    if (label != "N")
    {
      in.setstate(std::ios::failbit);
      return in;
    }
    long long numerator = 0;
    in >> numerator;
    in >> DelimiterIO{ ':' };
    in >> label;
    if (label != "D")
    {
      in.setstate(std::ios::failbit);
      return in;
    }
    unsigned long long denominator = 0;
    in >> denominator;
    in >> DelimiterIO{ ':' };
    in >> DelimiterIO{ ')' };
    if (denominator == 0)
    {
      in.setstate(std::ios::failbit);
      return in;
    }
    dest.value = { numerator, denominator };
    return in;
  }

  std::istream& operator>>(std::istream& in, StringIO&& dest)
  {
    std::istream::sentry sentry(in);
    if (!sentry)
    {
      return in;
    }
    in >> DelimiterIO{ '"' };
    std::getline(in, dest.value, '"');
    return in;
  }

  class IofGuard
  {
  public:
    IofGuard(std::basic_ios<char>& s) :
      s_(s),
      fill_(s.fill()),
      precision_(s.precision()),
      fmt_(s.flags())
    {
    }

    ~IofGuard()
    {
      s_.fill(fill_);
      s_.precision(precision_);
      s_.flags(fmt_);
    }

  private:
    std::basic_ios<char>& s_;
    char fill_;
    std::streamsize precision_;
    std::basic_ios<char>::fmtflags fmt_;
  };

  struct DataStruct
  {
    long long key1;
    std::pair<long long, unsigned long long> key2;
    std::string key3;
  };

  struct DataStructComparator
  {
    bool operator()(const DataStruct& a, const DataStruct& b) const
    {
      if (a.key1 != b.key1)
      {
        return a.key1 < b.key1;
      }
      double valA = static_cast<double>(a.key2.first)
                    / static_cast<double>(a.key2.second);
      double valB = static_cast<double>(b.key2.first)
                    / static_cast<double>(b.key2.second);
      if (std::fabs(valA - valB) > 1e-10)
      {
        return valA < valB;
      }
      return a.key3.length() < b.key3.length();
    }
  };

  std::istream& operator>>(std::istream& in, DataStruct& dest)
  {
    std::istream::sentry sentry(in);
    if (!sentry)
    {
      return in;
    }
    DataStruct temp;
    bool hasKey1 = false;
    bool hasKey2 = false;
    bool hasKey3 = false;
    in >> DelimiterIO{ '(' };
    in >> DelimiterIO{ ':' };
    while (in && (!hasKey1 || !hasKey2 || !hasKey3))
    {
      std::string label;
      in >> label;
      if (label == "key1" && !hasKey1)
      {
        in >> LongLongIO{ temp.key1 };
        in >> DelimiterIO{ ':' };
        hasKey1 = true;
      }
      else if (label == "key2" && !hasKey2)
      {
        in >> RationalIO{ temp.key2 };
        in >> DelimiterIO{ ':' };
        hasKey2 = true;
      }
      else if (label == "key3" && !hasKey3)
      {
        in >> StringIO{ temp.key3 };
        in >> DelimiterIO{ ':' };
        hasKey3 = true;
      }
      else
      {
        in.setstate(std::ios::failbit);
        return in;
      }
    }
    in >> DelimiterIO{ ')' };
    if (in && hasKey1 && hasKey2 && hasKey3)
    {
      dest = temp;
    }
    else
    {
      in.setstate(std::ios::failbit);
    }
    return in;
  }

  std::ostream& operator<<(std::ostream& out, const DataStruct& src)
  {
    std::ostream::sentry sentry(out);
    if (!sentry)
    {
      return out;
    }
    IofGuard guard(out);
    out << "(:key1 " << src.key1 << "ll";
    out << ":key2 (:N " << src.key2.first
        << ":D " << src.key2.second << ":)";
    out << ":key3 \"" << src.key3 << "\":)";
    return out;
  }
}

int main()
{
  using barinov::DataStruct;
  using barinov::DataStructComparator;

  std::vector<DataStruct> data;

  std::copy(
    std::istream_iterator<DataStruct>(std::cin),
    std::istream_iterator<DataStruct>(),
    std::back_inserter(data)
  );

  std::sort(data.begin(), data.end(), DataStructComparator());

  std::copy(
    data.begin(),
    data.end(),
    std::ostream_iterator<DataStruct>(std::cout, "\n")
  );

  return 0;
}

