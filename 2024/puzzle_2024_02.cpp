#include <2024/puzzle_2024_02.h>
#include <common.h>

namespace puzzle_2024_02 {

class Report {
public:
  Report(const QString &input) : m_levels{common::toIntValues(input, ' ')} {}

  Report(QVector<int> &&levels) : m_levels{levels} {}

  bool isSafe() const {
    const auto nb_levels = m_levels.size();
    if (nb_levels < 2) {
      return true;
    }
    const auto init_sign = m_levels[1] < m_levels[0] ? -1 : 1;
    for (auto i = 0; i + 1 < nb_levels; ++i) {
      const auto delta = m_levels[i + 1] - m_levels[i];
      const auto abs_delta = std::abs(delta);
      const auto sign = delta < 0 ? -1 : 1;
      if (abs_delta < 1 or abs_delta > 3 or sign != init_sign) {
        return false;
      }
    }
    return true;
  }

  bool isSafeWithDampener() const {
    for (auto i = 0; i < m_levels.size(); ++i) {
      auto copy = m_levels;
      copy.removeAt(i);
      if (Report(std::move(copy)).isSafe()) {
        return true;
      }
    }
    return false;
  }

private:
  QVector<int> m_levels;
};

class Reports {
public:
  Reports(const QString &input) {
    const auto lines = common::splitLines(input);
    m_levelss.reserve(lines.size());
    for (const auto &line : lines) {
      m_levelss.emplace_back(line);
    }
  }

  QString solveOne() const {
    auto sum = 0u;
    for (const auto &report : m_levelss) {
      if (report.isSafe()) {
        ++sum;
      }
    }
    return QString("%1").arg(sum);
  }

  QString solveTwo() const {
    auto sum = 0u;
    for (const auto &report : m_levelss) {
      if (report.isSafe() or report.isSafeWithDampener()) {
        ++sum;
      }
    }
    return QString("%1").arg(sum);
  }

private:
  std::vector<Report> m_levelss;
};

} // namespace puzzle_2024_02

void Solver_2024_02_1::solve(const QString &input) {
  const auto reports = puzzle_2024_02::Reports{input};
  emit finished(reports.solveOne());
}

void Solver_2024_02_2::solve(const QString &input) {
  const auto reports = puzzle_2024_02::Reports{input};
  emit finished(reports.solveTwo());
}
