#pragma once

#include <sstream>
#include <string>

namespace aoc {

short int toShortInt(const std::string &str, int base = 10);
int toInt(const std::string &str, int base = 10);
long int toLongInt(const std::string &str, int base = 10);
long long int toLongLongInt(const std::string &str, int base = 10);

unsigned short int toUShortInt(const std::string &str, int base = 10);
unsigned int toUInt(const std::string &str, int base = 10);
unsigned long int toULongInt(const std::string &str, int base = 10);
unsigned long long int toULongLongInt(const std::string &str, int base = 10);

float toFloat(const std::string &str);
double toDouble(const std::string &str);

template <typename T> std::string toString(const T &value) {
  auto stream = std::stringstream();
  stream << value;
  return stream.str();
}

} // namespace aoc
