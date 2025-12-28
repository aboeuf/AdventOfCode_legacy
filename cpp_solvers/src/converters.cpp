#include <converters.hpp>
#include <stdexcept>

namespace aoc {

short int toShortInt(const std::string &str, int base) {
  return static_cast<short>(toInt(str, base));
}

int toInt(const std::string &str, int base) {
  auto pos = std::size_t(0);
  const auto result = std::stoi(str, &pos, base);
  if (pos != std::size(str)) {
    throw std::runtime_error("cannot convert string \"" + str + "\" to int");
  }
  return result;
}

long int toLongInt(const std::string &str, int base) {
  auto pos = std::size_t(0);
  const auto result = std::stol(str, &pos, base);
  if (pos != std::size(str)) {
    throw std::runtime_error("cannot convert string \"" + str +
                             "\" to long int");
  }
  return result;
}

long long int toLongLongInt(const std::string &str, int base) {
  auto pos = std::size_t(0);
  const auto result = std::stoll(str, &pos, base);
  if (pos != std::size(str)) {
    throw std::runtime_error("cannot convert string \"" + str +
                             "\" to long long int");
  }
  return result;
}

unsigned short int toUShortInt(const std::string &str, int base) {
  return static_cast<unsigned short int>(toUInt(str, base));
}

unsigned int toUInt(const std::string &str, int base) {
  const auto signed_result = toLongInt(str, base);
  if (signed_result < 0) {
    throw std::runtime_error("cannot convert string \"" + str +
                             "\" to unsigned int (negative)");
  }
  return static_cast<unsigned int>(signed_result);
}

unsigned long int toULongInt(const std::string &str, int base) {
  auto pos = std::size_t(0);
  const auto result = std::stoul(str, &pos, base);
  if (pos != std::size(str)) {
    throw std::runtime_error("cannot convert string \"" + str +
                             "\" to unsigned long int");
  }
  return result;
}

unsigned long long int toULongLongInt(const std::string &str, int base) {
  auto pos = std::size_t(0);
  const auto result = std::stoull(str, &pos, base);
  if (pos != std::size(str)) {
    throw std::runtime_error("cannot convert string \"" + str +
                             "\" to unsigned long long int");
  }
  return result;
}

float toFloat(const std::string &str) {
  auto pos = std::size_t(0);
  const auto result = std::stof(str, &pos);
  if (pos != std::size(str)) {
    throw std::runtime_error("cannot convert string \"" + str + "\" to float");
  }
  return result;
}

double toDouble(const std::string &str) {
  auto pos = std::size_t(0);
  const auto result = std::stod(str, &pos);
  if (pos != std::size(str)) {
    throw std::runtime_error("cannot convert string \"" + str + "\" to double");
  }
  return result;
}

} // namespace aoc
