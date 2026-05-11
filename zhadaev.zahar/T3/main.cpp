#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <numeric>
#include <iomanip>
#include <iterator>
#include <algorithm>
#include <cctype>
#include <cmath>

namespace T3
{
  struct Point { int x, y; };

  struct Shape
  {
    std::vector<Point> points;
  };

  struct Mark
  {
    char expected;
    Mark(char c) : expected(c) {}
  };

  std::istream& operator>>(std::istream& stream, Mark&& m)
  {
    char actual;
    stream >> actual;
    if (stream && actual != m.expected)
    {
      stream.setstate(std::ios::failbit);
    }
    return stream;
  }

  std::istream& operator>>(std::istream& is, Point& p)
  {
    is >> Mark('(') >> p.x >> Mark(';') >> p.y >> Mark(')');
    return is;
  }

  std::istream& operator>>(std::istream& is, Shape& sh)
  {
    std::istream::sentry s(is);
    size_t n;
    if (!s || !(is >> n) || n < 3)
    {
      is.setstate(std::ios::failbit);
      return is;
    }

    std::vector<Point> v(n);
    for (auto& pt : v)
    {
      if (!(is >> pt))
      {
        is.setstate(std::ios::failbit);
        return is;
      }
    }
    sh.points = std::move(v);
    return is;
  }

  double calculateArea(const Shape& s)
  {
    const auto& d = s.points;
    double res = 0.0;
    for (size_t i = 0; i < d.size(); ++i)
    {
      const Point& cur = d[i];
      const Point& nxt = d[(i + 1) % d.size()];
      res += (1.0 * cur.x * nxt.y - 1.0 * nxt.x * cur.y);
    }
    return std::abs(res) / 2.0;
  }

  int findDir(const Point& a, const Point& b, const Point& c)
  {
    long long v = 1LL * (b.y - a.y) * (c.x - b.x) - 1LL * (b.x - a.x) * (c.y - b.y);
    return (v == 0) ? 0 : ((v > 0) ? 1 : 2);
  }

  bool hasCross(const Point& a, const Point& b, const Point& c, const Point& d)
  {
    int d1 = findDir(a, b, c);
    int d2 = findDir(a, b, d);
    int d3 = findDir(c, d, a);
    int d4 = findDir(c, d, b);
    return (d1 != d2 && d3 != d4);
  }

  bool isSame(const Shape& f, const Shape& s)
  {
    if (f.points.size() != s.points.size()) return false;
    auto cmp = [](const Point& p1, const Point& p2) {
      return p1.x == p2.x && p1.y == p2.y;
    };
    return std::equal(f.points.begin(), f.points.end(), s.points.begin(), cmp);
  }

  struct AreaCalc
  {
    std::string type;
    size_t val;
    AreaCalc(std::string t, size_t v = 0) : type(t), val(v) {}
    double operator()(double acc, const Shape& s) const
    {
      bool ok = false;
      size_t sz = s.points.size();
      if (type == "EVEN") ok = (sz % 2 == 0);
      else if (type == "ODD") ok = (sz % 2 != 0);
      else if (type == "NUM") ok = (sz == val);
      else if (type == "MEAN") ok = true;
      return ok ? (acc + calculateArea(s)) : acc;
    }
  };

  struct CrossChecker
  {
    const Shape& base;
    CrossChecker(const Shape& b) : base(b) {}
    bool operator()(const Shape& target) const
    {
      for (size_t i = 0; i < target.points.size(); ++i) {
        for (size_t j = 0; j < base.points.size(); ++j) {
          if (hasCross(target.points[i], 
              target.points[(i + 1) % target.points.size()],
              base.points[j], 
              base.points[(j + 1) % base.points.size()])) return true;
        }
      }
      return false;
    }
  };

  struct DuplicateRemover
  {
    const Shape& sample;
    DuplicateRemover(const Shape& s) : sample(s) {}
    bool operator()(const Shape& x, const Shape& y) const {
      return isSame(x, sample) && isSame(y, sample);
    }
  };
}

int main()
{
  std::vector<T3::Shape> data;
  while (std::cin >> std::ws)
  {
    if (!std::isdigit(std::cin.peek())) break;
    T3::Shape temp;
    if (std::cin >> temp) data.push_back(std::move(temp));
    else
    {
      std::cin.clear();
      std::string trash;
      std::getline(std::cin, trash);
    }
  }

  std::string cmd;
  while (std::cin >> cmd)
  {
    try
    {
      if (cmd == "AREA")
      {
        std::string arg;
        std::cin >> arg;
        std::cout << std::fixed << std::setprecision(1);
        if (arg == "EVEN" || arg == "ODD")
        {
          std::cout << std::accumulate(data.begin(), data.end(),
            0.0, T3::AreaCalc(arg)) << "\n";
        }
        else if (arg == "MEAN")
        {
          if (data.empty()) throw std::logic_error("");
          double sum = std::accumulate(data.begin(), data.end(),
            0.0, T3::AreaCalc("MEAN"));
          std::cout << sum / data.size() << "\n";
        }
        else
        {
          size_t n = std::stoul(arg);
          if (n < 3) throw std::logic_error("");
          std::cout << std::accumulate(data.begin(), data.end(),
            0.0, T3::AreaCalc("NUM", n)) << "\n";
        }
      }
      else if (cmd == "MAX" || cmd == "MIN")
      {
        std::string mode;
        std::cin >> mode;
        if (data.empty()) throw std::logic_error("");
        auto cmpA = [](const T3::Shape& a, const T3::Shape& b) {
          return calculateArea(a) < calculateArea(b);
        };
        auto cmpV = [](const T3::Shape& a, const T3::Shape& b) {
          return a.points.size() < b.points.size();
        };

        if (mode == "AREA")
        {
          auto it = (cmd == "MAX") ? 
            std::max_element(data.begin(), data.end(), cmpA) :
            std::min_element(data.begin(), data.end(), cmpA);
          std::cout << std::fixed << std::setprecision(1) <<
            T3::calculateArea(*it) << "\n";
        }
        else if (mode == "VERTICES")
        {
          auto it = (cmd == "MAX") ? 
            std::max_element(data.begin(), data.end(), cmpV) :
            std::min_element(data.begin(), data.end(), cmpV);
          std::cout << it->points.size() << "\n";
        }
      }
      else if (cmd == "INTERSECTIONS")
      {
        T3::Shape target;
        if (!(std::cin >> target)) throw std::logic_error("");
        std::cout << std::count_if(data.begin(), data.end(),
          T3::CrossChecker(target)) << "\n";
      }
      else if (cmd == "RMECHO")
      {
        T3::Shape sample;
        if (!(std::cin >> sample)) throw std::logic_error("");
        size_t before = data.size();
        auto it = std::unique(data.begin(), data.end(),
          T3::DuplicateRemover(sample));
        data.erase(it, data.end());
        std::cout << before - data.size() << "\n";
      }
      else throw std::logic_error("");
    }
    catch (...)
    {
      std::cout << "<INVALID COMMAND>\n";
      std::cin.clear();
      std::string dummy;
      std::getline(std::cin, dummy);
    }
  }
  return 0;
}
