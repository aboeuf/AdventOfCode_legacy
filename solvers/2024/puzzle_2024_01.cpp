#include <solvers/2024/puzzle_2024_01.h>
#include <solvers/common.h>

namespace puzzle_2024_01 {

class Maps {
public:
  Maps(const QString &input) {
    auto lines = common::splitLines(input);
    m_first_list.reserve(lines.size());
    m_second_list.reserve(lines.size());
    for (const auto &line : lines) {
      const auto values = common::toVecUInt(line, ' ');
      if (values.size() != 2) {
        common::throwInvalidArgumentError("cannot parse values");
      }
      m_first_list.emplace_back(values[0]);
      m_second_list.emplace_back(values[1]);
    }
    std::sort(std::begin(m_first_list), std::end(m_first_list));
    std::sort(std::begin(m_second_list), std::end(m_second_list));
  }

  QString solveOne() const {
    auto sum = 0;
    for (auto i = 0u; i < std::size(m_first_list); ++i) {
      sum += std::abs(m_first_list[i] - m_second_list[i]);
    }
    return QString("%1").arg(sum);
  }

  QString solveTwo() const {
    auto counter = std::unordered_map<int, int>{};
    for (auto value : m_second_list) {
      counter.insert(std::make_pair(value, 0)).first->second += 1;
    }
    auto sum = 0;
    for (auto value : m_first_list) {
      const auto it = counter.find(value);
      if (it != std::cend(counter)) {
        sum += value * it->second;
      }
    }
    return QString("%1").arg(sum);
  }

private:
  std::vector<int> m_first_list;
  std::vector<int> m_second_list;
};

} // namespace puzzle_2024_01

void Solver_2024_01_1::solve(const QString &input) {
  const auto maps = puzzle_2024_01::Maps{input};
  emit finished(maps.solveOne());
}

void Solver_2024_01_2::solve(const QString &input) {
  const auto maps = puzzle_2024_01::Maps{input};
  const auto res = maps.solveTwo();
  emit finished(res);
}
