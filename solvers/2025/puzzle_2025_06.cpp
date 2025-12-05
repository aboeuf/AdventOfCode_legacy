#include <solvers/2025/puzzle_2025_06.h>
#include <solvers/common.h>

namespace puzzle_2025_06 {

using Int = long long unsigned int;

class MathHomework {
public:
  MathHomework(const QString &input) {
    m_lines = common::splitLines(input);
    const auto nb_lines = m_lines.size();
    if (nb_lines < 2) {
      throw std::invalid_argument("bad number of lines");
    }
    m_length = nb_lines - 1;
    m_values.reserve(m_length);
    for (auto line : m_lines) {
      while (line.contains("  ")) {
        line.replace("  ", " ");
      }
      line.replace(" ", ",");
      if (static_cast<int>(m_values.size()) < m_length) {
        const auto values = common::toVecULongLong(line);
        if (m_width == 0) {
          m_width = values.size();
        } else if (values.size() != m_width) {
          throw std::invalid_argument("width mismatch");
        }
        m_values.emplace_back();
        m_values.back().reserve(m_width);
        for (const auto value : values) {
          m_values.back().emplace_back(value);
        }
      } else {
        line.remove(',');
        m_operators = line;
        if (m_operators.size() != m_width) {
          throw std::invalid_argument("wrong size for operators string");
        }
      }
    }
  }

  QString solveOne() const {
    auto grand_total = Int{0};
    for (auto j = 0; j < m_width; ++j) {
      auto solution = m_operators[j] == '*' ? Int{1} : Int{0};
      for (auto i = 0; i < m_length; ++i) {
        if (m_operators[j] == '*') {
          solution *= m_values[i][j];
        } else if (m_operators[j] == '+') {
          solution += m_values[i][j];
        } else {
          throw std::invalid_argument("unknown operator");
        }
      }
      grand_total += solution;
    }
    return QString("%1").arg(grand_total);
  }

  QString solveTwo() const {
    auto grand_total = Int{0};
    auto column = 0;
    for (const auto &op : m_operators) {
      auto solution = op == '*' ? Int{1} : Int{0};
      for (;;) {
        auto value = Int{0};
        auto power = Int{1};
        for (auto i = m_length - 1; i >= 0; --i) {
          const auto &c = m_lines[i].at(column);
          if (c.isDigit()) {
            value += power * QString(c).toULongLong();
            power *= Int{10};
          }
        }
        ++column;
        if (value == Int{0}) {
          break;
        }
        if (op == '*') {
          solution *= value;
        } else if (op == '+') {
          solution += value;
        } else {
          throw std::invalid_argument("unknown operator");
        }
      }
      grand_total += solution;
    }
    return QString("%1").arg(grand_total);
  }

private:
  int m_length{0};
  int m_width{0};
  std::vector<std::vector<Int>> m_values;
  QString m_operators;
  QStringList m_lines;
};

} // namespace puzzle_2025_06

void Solver_2025_06_1::solve(const QString &input) {
  const auto homework = puzzle_2025_06::MathHomework(input);
  emit finished(homework.solveOne());
}

void Solver_2025_06_2::solve(const QString &input) {
  const auto homework = puzzle_2025_06::MathHomework(input);
  emit finished(homework.solveTwo());
}
