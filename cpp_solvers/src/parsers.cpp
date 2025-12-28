#include <parsers.hpp>

namespace aoc {

std::deque<std::string> split(const std::string &str, char delimiter,
                              bool remove_empty) {

  auto result = std::deque<std::string>();
  const auto n = std::size(str);
  auto pos = std::size_t(0);
  auto len = std::size_t(0);
  while (pos + len < n) {
    if (str[pos + len] == delimiter) {
      if (not remove_empty or len > std::size_t(0)) {
        result.emplace_back(str.substr(pos, len));
      }
      pos = pos + len + std::size_t(1);
      len = std::size_t(0);
    } else {
      ++len;
    }
  }
  if (str.back() != delimiter and (not remove_empty or len > std::size_t(0))) {
    result.emplace_back(str.substr(pos, len));
  }
  return result;
}

std::deque<std::string> splitLines(const std::string &str, bool remove_empty) {
  return split(str, '\n', remove_empty);
}

std::deque<short int> splitToShortInt(const std::string &str, char delimiter) {
  auto result = std::deque<short int>();
  for (const auto &substr : split(str, delimiter)) {
    result.emplace_back(toShortInt(substr));
  }
  return result;
}

std::deque<int> splitToInt(const std::string &str, char delimiter) {
  auto result = std::deque<int>();
  for (const auto &substr : split(str, delimiter)) {
    result.emplace_back(toInt(substr));
  }
  return result;
}

std::deque<long int> splitToLongInt(const std::string &str, char delimiter) {
  auto result = std::deque<long int>();
  for (const auto &substr : split(str, delimiter)) {
    result.emplace_back(toLongInt(substr));
  }
  return result;
}

std::deque<long long int> splitToLongLongInt(const std::string &str,
                                             char delimiter) {
  auto result = std::deque<long long int>();
  for (const auto &substr : split(str, delimiter)) {
    result.emplace_back(toLongLongInt(substr));
  }
  return result;
}

std::deque<unsigned short int> splitToUShortInt(const std::string &str,
                                                char delimiter) {
  auto result = std::deque<unsigned short int>();
  for (const auto &substr : split(str, delimiter)) {
    result.emplace_back(toUShortInt(substr));
  }
  return result;
}

std::deque<unsigned int> splitToUInt(const std::string &str, char delimiter) {
  auto result = std::deque<unsigned int>();
  for (const auto &substr : split(str, delimiter)) {
    result.emplace_back(toUInt(substr));
  }
  return result;
}

std::deque<unsigned long int> splitToULongInt(const std::string &str,
                                              char delimiter) {
  auto result = std::deque<unsigned long int>();
  for (const auto &substr : split(str, delimiter)) {
    result.emplace_back(toULongInt(substr));
  }
  return result;
}

std::deque<unsigned long long int> splitToULongLongInt(const std::string &str,
                                                       char delimiter) {
  auto result = std::deque<unsigned long long int>();
  for (const auto &substr : split(str, delimiter)) {
    result.emplace_back(toULongLongInt(substr));
  }
  return result;
}

std::deque<float> splitToFloat(const std::string &str, char delimiter) {
  auto result = std::deque<float>();
  for (const auto &substr : split(str, delimiter)) {
    result.emplace_back(toFloat(substr));
  }
  return result;
}

std::deque<double> splitToDouble(const std::string &str, char delimiter) {
  auto result = std::deque<double>();
  for (const auto &substr : split(str, delimiter)) {
    result.emplace_back(toDouble(substr));
  }
  return result;
}

} // namespace aoc
