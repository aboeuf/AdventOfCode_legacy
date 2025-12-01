#include <solvers/common.h>
#include <solvers/2024/puzzle_2024_07.h>

namespace puzzle_2024_07 {

using Int = unsigned long long;

inline Int pow(Int base, Int exp) {
  if (exp <= Int{0}) {
    return Int{1};
  }
  if (exp == Int{1}) {
    return base;
  }
  return base * pow(base, exp - Int{1});
}

QStringList makeOperators(int size, const QString &operators) {
  static auto cache =
      std::unordered_map<int, std::unordered_map<QString, QStringList>>{};
  const auto base = operators.size();
  if (base > 10) {
    throw std::runtime_error("puzzle_2024_07::makeOperators: not implemented "
                             "for more than 10 operators");
  }
  auto [it, is_new] =
      cache[size].insert(std::make_pair(operators, QStringList{}));
  if (is_new) {
    auto ops = QString(size, '#');
    const auto nb_lines = pow(base, size);
    for (auto i = Int{0}; i < nb_lines; ++i) {
      const auto change_base = QString("%1").arg(i, size, base, QChar('0'));
      for (auto j = 0; j < size; ++j) {
        ops[j] = operators[QString(change_base[j]).toUInt()];
      }
      it->second << ops;
    }
  }
  return it->second;
}

class Equation {
public:
  Equation(const QString &input) {
    const auto colon_split = common::splitValues(input, ':');
    if (colon_split.size() != 2) {
      throw std::runtime_error(
          "puzzle_2024_07::Equation: failed to split line");
    }
    auto ok = true;
    m_test_value = colon_split.front().toULongLong(&ok);
    if (not ok) {
      throw std::runtime_error(
          "puzzle_2024_07::Equation: failed to read test value");
    }
    m_operands = common::toVecULongLong(colon_split.back(), ' ');
    if (m_operands.size() < 2) {
      throw std::runtime_error(
          "puzzle_2024_07::Equation: failed to read operands");
    }
  }

  Int evaluate(const QString &operators) const {
    if (operators.size() + 1 != m_operands.size()) {
      throw std::invalid_argument(
          QString("puzzle_2024_07::Equation::evaluate: bad size for operators "
                  "\"%1\" (expected size is %2)")
              .arg(operators)
              .arg(m_operands.size() - 1)
              .toStdString());
    }
    auto result = m_operands.front();
    for (auto i = 0; i < operators.size(); ++i) {
      if (operators[i] == '+') {
        result += m_operands[i + 1];
      } else if (operators[i] == '*') {
        result *= m_operands[i + 1];
      } else if (operators[i] == '|') {
        const auto nb_digits =
            static_cast<Int>(QString("%1").arg(m_operands[i + 1]).size());
        result *= pow(Int{10}, nb_digits);
        result += m_operands[i + 1];
      } else {
        throw std::invalid_argument(
            QString("puzzle_2024_07::Equation::evaluate: unknown operator '%1'")
                .arg(operators[i])
                .toStdString());
      }
    }
    return result;
  }

  Int solveOne() const {
    const auto operators = makeOperators(m_operands.size() - 1, "+*");
    for (const auto &ops : operators) {
      if (evaluate(ops) == m_test_value) {
        return m_test_value;
      }
    }
    return Int{0};
  }

  Int solveTwo() const {
    const auto operators = makeOperators(m_operands.size() - 1, "+*|");
    for (const auto &ops : operators) {
      if (evaluate(ops) == m_test_value) {
        return m_test_value;
      }
    }
    return Int{0};
  }

private:
  Int m_test_value;
  QVector<Int> m_operands;
};

class Equations {
public:
  Equations(const QString &input) {
    const auto lines = common::splitLines(input);
    m_equations.reserve(lines.size());
    for (const auto &line : lines)
      m_equations.emplace_back(line);
  }

  QString solveOne() const {
    auto result = Int{0};
    for (const auto &equation : m_equations) {
      result += equation.solveOne();
    }
    return QString("%1").arg(result);
  }

  QString solveTwo() const {
    auto result = Int{0};
    for (const auto &equation : m_equations) {
      result += equation.solveTwo();
    }
    return QString("%1").arg(result);
  }

private:
  std::vector<Equation> m_equations;
};

} // namespace puzzle_2024_07

void Solver_2024_07_1::solve(const QString &input) {
  const auto equations = puzzle_2024_07::Equations{input};
  emit finished(equations.solveOne());
}

void Solver_2024_07_2::solve(const QString &input) {
  const auto equations = puzzle_2024_07::Equations{input};
  emit finished(equations.solveTwo());
}
