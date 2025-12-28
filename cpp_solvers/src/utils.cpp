#include <utils.hpp>

namespace aoc {

std::string repeatedString(const std::string &str, std::size_t n) {
  auto res = std::string{};
  for (auto i = 0u; i < n; ++i) {
    res += str;
  }
  return res;
}

} // namespace aoc
