#include <2017/puzzle_2017_02.h>
#include <common.h>
#include <limits>

namespace puzzle_2017_02 {

class Line {
public:
  Line(const QString &input) : m_values{common::toUInt(input, '\t')} {}

  uint partOneCheckSum() const {
    auto min_value = std::numeric_limits<uint>::max();
    auto max_value = 0u;
    for (const auto value : m_values) {
      min_value = std::min(min_value, value);
      max_value = std::max(max_value, value);
    }
    if (max_value < min_value) {
      common::throwRunTimeError(
          "puzzle_2017_02::Line::partOneCheckSum: max_value < min_value");
    }
    return max_value - min_value;
  }

  uint partTwoCheckSum() const {
    for (auto i = 0; i + 1 < m_values.size(); ++i) {
      for (auto j = i + 1; j < m_values.size(); ++j) {
        if (m_values[i] < m_values[j]) {
          if (m_values[j] % m_values[i] == 0u) {
            return m_values[j] / m_values[i];
          }
        } else {
          if (m_values[i] % m_values[j] == 0u) {
            return m_values[i] / m_values[j];
          }
        }
      }
    }
    common::throwRunTimeError(
        "puzzle_2017_02::Line::partTwoCheckSum: no solution");
    return 0u;
  }

private:
  QVector<uint> m_values;
};

class Spreadsheet {
public:
  Spreadsheet(const QString &input) {
    const auto lines = common::splitLines(input, true);
    m_lines.reserve(lines.size());
    for (const auto& line : lines) {
      m_lines.emplace_back(line);
    }
  }

  QString solveOne() const {
    auto checksum = 0u;
    for (const auto &line : m_lines) {
      checksum += line.partOneCheckSum();
    }
    return QString("%1").arg(checksum);
  }

  QString solveTwo() const {
    auto checksum = 0u;
    for (const auto &line : m_lines) {
      checksum += line.partTwoCheckSum();
    }
    return QString("%1").arg(checksum);
  }

private:
  std::vector<Line> m_lines{};
};

} // namespace puzzle_2017_02

void Solver_2017_02_1::solve(const QString &input) {
  const auto speadsheat = puzzle_2017_02::Spreadsheet(input);
  emit finished(speadsheat.solveOne());
}

void Solver_2017_02_2::solve(const QString &input) {
  const auto speadsheat = puzzle_2017_02::Spreadsheet(input);
  emit finished(speadsheat.solveTwo());
}
