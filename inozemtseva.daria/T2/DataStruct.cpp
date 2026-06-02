#include "DataStruct.h"
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <cctype>
#include <stdexcept>

namespace {
  constexpr double kFloatEpsilon = 1e-9;

  std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return {};
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
  }

  bool parseKey1(const std::string& rawValue, unsigned long long& val) {
    std::string value = trim(rawValue);
    if (value.size() < 2) return false;

    std::string prefix = value.substr(0, 2);
    if (prefix != "0x" && prefix != "0X") return false;

    try {
      size_t idx = 0;
      val = std::stoull(value.substr(2), &idx, 16);
      if (idx != value.size() - 2) return false;
    } catch (const std::invalid_argument&) {
      return false;
    } catch (const std::out_of_range&) {
      return false;
    }
    return true;
  }

  bool parseKey2(const std::string& rawValue, std::complex<double>& val) {
    std::string value = trim(rawValue);
    if (value.size() < 5 || value.substr(0, 2) != "#c" || value.back() != ')') {
      return false;
    }

    std::string inner = value.substr(2);
    if (inner.front() != '(') return false;
    inner = inner.substr(1, inner.size() - 2);

    size_t dashPos = inner.find('-', 1);
    if (dashPos != std::string::npos && inner.find(' ') == std::string::npos) {
      inner.insert(dashPos, " ");
    }

    std::istringstream ss(inner);
    double realPart = 0.0, imagPart = 0.0;
    if (!(ss >> realPart >> imagPart)) return false;

    std::string remainder;
    if (ss >> remainder) return false;

    val = std::complex<double>(realPart, imagPart);
    return true;
  }

  bool parseKey3(const std::string& rawValue, std::string& val) {
    std::string value = trim(rawValue);
    if (value.size() < 2 || value.front() != '"' || value.back() != '"') {
      return false;
    }
    val = value.substr(1, value.size() - 2);
    return true;
  }

  bool parseContent(const std::string& content, DataStruct& d) {
    bool found_key1 = false, found_key2 = false, found_key3 = false;
    size_t pos = 0;

    auto skipWhitespace = [&content](size_t& p) {
      while (p < content.size() && std::isspace(static_cast<unsigned char>(content[p]))) {
        ++p;
      }
    };

    while (pos < content.size()) {
      skipWhitespace(pos);
      if (pos >= content.size()) break;

      if (content[pos] == ':') {
        ++pos;
        skipWhitespace(pos);
      }

      size_t keyStart = pos;
      while (pos < content.size() && std::isalnum(static_cast<unsigned char>(content[pos]))) {
        ++pos;
      }
      std::string key = content.substr(keyStart, pos - keyStart);

      if (key.empty()) {
        break;
      }

      skipWhitespace(pos);

      if (pos < content.size() && content[pos] == ':') {
        ++pos;
        skipWhitespace(pos);
      }

      size_t valStart = pos;
      while (pos < content.size() && content[pos] != ':') {
        ++pos;
      }
      std::string rawValue = content.substr(valStart, pos - valStart);
      rawValue = trim(rawValue);

      if (key == "key1") {
        if (!parseKey1(rawValue, d.key1_)) {
          return false;
        }
        if (found_key1) return false;
        found_key1 = true;
      } else if (key == "key2") {
        if (!parseKey2(rawValue, d.key2_)) {
          return false;
        }
        if (found_key2) return false;
        found_key2 = true;
      } else if (key == "key3") {
        if (!parseKey3(rawValue, d.key3_)) {
          return false;
        }
        if (found_key3) return false;
        found_key3 = true;
      } else {
        return false;
      }
    }

    return found_key1 && found_key2 && found_key3;
  }
}

std::istream& operator>>(std::istream& in, DataStruct& d)
{
  std::string line;

  while (std::getline(in, line)) {
    std::string content = trim(line);

    if (content.size() < 4) {
      continue;
    }
    if (content.substr(0, 2) != "(:") {
      continue;
    }
    if (content.substr(content.size() - 2) != ":)") {
      continue;
    }

    content = content.substr(2, content.size() - 4);

    if (parseContent(content, d)) {
      return in;
    }
  }

  in.setstate(std::ios::failbit);
  return in;
}

std::ostream& operator<<(std::ostream& out, const DataStruct& d)
{
  std::stringstream hexStream;
  hexStream << std::hex << std::uppercase << d.key1_;

  std::ios oldState(nullptr);
  oldState.copyfmt(out);

  out << std::fixed << std::setprecision(1);

  out << "(:key1 0x" << hexStream.str()
  << ":key2 #c(" << d.key2_.real() << " " << d.key2_.imag()
  << "):key3 \"" << d.key3_ << "\":)";

  out.copyfmt(oldState);
  return out;
}

bool compareDataStructs(const DataStruct& a, const DataStruct& b)
{
  if (a.key1_ != b.key1_) {
    return a.key1_ < b.key1_;
  }

  double modulusA = std::abs(a.key2_);
  double modulusB = std::abs(b.key2_);

  if (std::abs(modulusA - modulusB) > kFloatEpsilon) {
    return modulusA < modulusB;
  }

  return a.key3_.length() < b.key3_.length();
}
