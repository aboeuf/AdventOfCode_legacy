#include <solvers/2023/puzzle_2023_13.h>
#include <solvers/common.h>

namespace puzzle_2023_13 {

class Pattern {
public:
  Pattern(const QStringList &lines, QStringList::iterator &end) {
    auto start = end;
    for (; end != std::end(lines) and not end->isEmpty(); ++end) {
      ++m_lenght;
    }
    m_data.reserve(m_lenght);
    m_width = start->size();
    for (auto it = start; it != end; ++it) {
      if (it->size() != m_width)
        common::throwInvalidArgumentError("incoherent width");
      m_data.emplace_back();
      m_data.back().reserve(m_width);
      for (const auto &c : *it)
        m_data.back().emplace_back(c == '.');
    }
  }

  bool isHorizontalMirrorLine(int index, uint expected_nb_errors) const {
    if (index < 0 or index + 1 >= m_lenght)
      return false;
    auto nb_errors = 0u;
    for (auto j = 0; j < m_width; ++j) {
      auto top = index;
      auto bottom = index + 1;
      while (top >= 0 and bottom < m_lenght) {
        if (m_data[top][j] != m_data[bottom][j])
          ++nb_errors;
        if (nb_errors > expected_nb_errors)
          return false;
        --top;
        ++bottom;
      }
    }
    return nb_errors == expected_nb_errors;
  }

  int findHorizontalMirrorLine(uint expected_nb_errors) const {
    for (auto i = 0; i + 1 < m_lenght; ++i)
      if (isHorizontalMirrorLine(i, expected_nb_errors))
        return i;
    return -1;
  }

  bool isVerticalMirrorLine(int index, uint expected_nb_errors) const {
    if (index < 0 or index + 1 >= m_width)
      return false;
    auto nb_errors = 0u;
    for (auto i = 0; i < m_lenght; ++i) {
      auto left = index;
      auto rigth = index + 1;
      while (left >= 0 and rigth < m_width) {
        if (m_data[i][left] != m_data[i][rigth])
          ++nb_errors;
        if (nb_errors > expected_nb_errors)
          return false;
        --left;
        ++rigth;
      }
    }
    return nb_errors == expected_nb_errors;
  }

  int findVerticalMirrorLine(uint expected_nb_errors) const {
    for (auto j = 0; j + 1 < m_width; ++j)
      if (isVerticalMirrorLine(j, expected_nb_errors))
        return j;
    return -1;
  }

  uint reflexionCode(uint expected_nb_errors) const {
    const auto horizontal_line =
        static_cast<uint>(findHorizontalMirrorLine(expected_nb_errors) + 1);
    if (horizontal_line != 0u)
      return 100u * horizontal_line;
    return static_cast<uint>(findVerticalMirrorLine(expected_nb_errors) + 1);
  }

private:
  std::vector<std::vector<bool>> m_data;
  int m_lenght{0};
  int m_width{0};
};

class Patterns {
public:
  Patterns(const QString &input) {
    auto lines = common::splitLines(input);
    auto nb_patterns = 0u;
    for (const auto &line : lines)
      if (line.isEmpty())
        ++nb_patterns;
    ++nb_patterns;
    m_patterns.reserve(nb_patterns);
    auto it = std::begin(lines);
    for (; it != std::end(lines);) {
      for (; it != std::end(lines) and it->isEmpty(); ++it) {
      }
      m_patterns.emplace_back(lines, it);
    }
  }

  QString solve(uint expected_nb_errors) const {
    auto sum = 0u;
    for (auto &pattern : m_patterns)
      sum += pattern.reflexionCode(expected_nb_errors);
    return QString{"%1"}.arg(sum);
  }

private:
  std::vector<Pattern> m_patterns;
};

} // namespace puzzle_2023_13

void Solver_2023_13_1::solve(const QString &input) {
  const auto patterns = puzzle_2023_13::Patterns{input};
  emit finished(patterns.solve(0u));
}

void Solver_2023_13_2::solve(const QString &input) {
  const auto patterns = puzzle_2023_13::Patterns{input};
  emit finished(patterns.solve(1u));
}
