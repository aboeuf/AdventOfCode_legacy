#include <solvers.hpp>

namespace puzzle_2025_01 {

class Document {
public:
  Document(const std::string &input) {
    auto dial_position = 50;
    const auto lines = aoc::splitLines(input);
    for (const auto &line : lines) {
      const auto sign =
          line.front() == 'R' ? 1 : (line.front() == 'L' ? -1 : 0);
      if (sign == 0) {
        throw std::invalid_argument("invalid line \"" + line +
                                    "\" (bad direction)");
      }
      const auto increment = aoc::toInt(line.substr(1));
      for (auto i = 0; i < increment; ++i) {
        dial_position = (dial_position + sign) % 100;
        if (dial_position == 0) {
          ++m_second_secret_code;
        }
      }
      if (dial_position == 0) {
        ++m_first_secret_code;
      }
    }
  }

  uint firstSecretCode() const { return m_first_secret_code; }
  uint secondSecretCode() const { return m_second_secret_code; }

private:
  uint m_first_secret_code{0u};
  uint m_second_secret_code{0u};
};

} // namespace puzzle_2025_01

namespace aoc {

Result solve_2025_01(const std::string &input) {
  const auto document = puzzle_2025_01::Document(input);
  auto res = Result();
  res.part_one_solution = std::to_string(document.firstSecretCode());
  res.part_two_solution = std::to_string(document.secondSecretCode());
  return res;
}

} // namespace aoc
