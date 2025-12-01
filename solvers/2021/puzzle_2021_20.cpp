#include <solvers/common.h>
#include <set>
#include <solvers/2021/puzzle_2021_20.h>

namespace puzzle_2021_20 {

using Int = long long int;

struct Pixel {
  Pixel(Int x, Int y) : m_x{x}, m_y{y} {}
  bool operator<(const Pixel &other) const {
    if (m_x == other.m_x)
      return m_y < other.m_y;
    return m_x < other.m_x;
  }
  Int m_x;
  Int m_y;
};

class Image {
public:
  Image(const QString &input) {
    const auto &lines = common::splitLines(input);
    if (lines.empty() or lines.front().size() != 512)
      return;
    for (auto i = 0; i < 512; ++i)
      m_enhancement_algorithm[i] = (lines.front()[i] == QChar('#'));
    auto x = Int{0};
    for (auto it = std::next(std::begin(lines)); it != std::end(lines); ++it) {
      if (it->isEmpty())
        continue;
      for (auto y = Int{0}; y < it->size(); ++y) {
        if (it->at(y) == QChar('#')) {
          m_different_than_outside.insert(Pixel(x, y));
          m_x_min = std::min(m_x_min, x);
          m_x_max = std::max(m_x_max, x);
          m_y_min = std::min(m_y_min, y);
          m_y_max = std::max(m_y_max, y);
        }
      }
      ++x;
    }
  }

  QString solve(uint nb_steps) {
    for (auto i = 0u; i < nb_steps; ++i)
      enhanceOnce();
    return QString("%1").arg(m_different_than_outside.size());
  }

private:
  bool isLit(Int x, Int y) const {
    if (m_x_min <= x and x <= m_x_max and m_y_min <= y and y <= m_y_max)
      return m_different_than_outside.find(Pixel(x, y)) ==
                     std::end(m_different_than_outside)
                 ? m_outside_is_lit
                 : not m_outside_is_lit;
    return m_outside_is_lit;
  }

  uint getIndex(Int x, Int y) const {
    auto bin = QString{};
    for (auto i = x - 1; i <= x + 1; ++i)
      for (auto j = y - 1; j <= y + 1; ++j)
        bin.push_back(isLit(i, j) ? '1' : '0');
    return bin.toUInt(nullptr, 2);
  }

  void enhanceOnce() {
    if (m_different_than_outside.empty())
      return;
    auto next_outside_is_lit =
        m_enhancement_algorithm[m_outside_is_lit ? 511 : 0];
    auto next_different_than_outside = std::set<Pixel>{};
    auto next_x_min = std::numeric_limits<Int>::max();
    auto next_x_max = std::numeric_limits<Int>::min();
    auto next_y_min = std::numeric_limits<Int>::max();
    auto next_y_max = std::numeric_limits<Int>::min();
    for (auto x = m_x_min - 1; x <= m_x_max + 1; ++x) {
      for (auto y = m_y_min - 1; y <= m_y_max + 1; ++y) {
        if (m_enhancement_algorithm[getIndex(x, y)] != next_outside_is_lit) {
          next_different_than_outside.insert(Pixel(x, y));
          next_x_min = std::min(next_x_min, x);
          next_x_max = std::max(next_x_max, x);
          next_y_min = std::min(next_y_min, y);
          next_y_max = std::max(next_y_max, y);
        }
      }
    }
    std::swap(m_different_than_outside, next_different_than_outside);
    m_x_min = next_x_min;
    m_x_max = next_x_max;
    m_y_min = next_y_min;
    m_y_max = next_y_max;
    m_outside_is_lit = next_outside_is_lit;
  }

  std::set<Pixel> m_different_than_outside{};
  std::array<bool, 512> m_enhancement_algorithm{};
  Int m_x_min{std::numeric_limits<Int>::max()};
  Int m_x_max{std::numeric_limits<Int>::min()};
  Int m_y_min{std::numeric_limits<Int>::max()};
  Int m_y_max{std::numeric_limits<Int>::min()};
  bool m_outside_is_lit{false};
};

} // namespace puzzle_2021_20

void Solver_2021_20_1::solve(const QString &input) {
  emit finished(puzzle_2021_20::Image(input).solve(2));
}

void Solver_2021_20_2::solve(const QString &input) {
  emit finished(puzzle_2021_20::Image(input).solve(50));
}
