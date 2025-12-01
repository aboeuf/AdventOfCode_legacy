#include <QRegExp>
#include <solvers/common.h>
#include <solvers/2024/puzzle_2024_13.h>

namespace puzzle_2024_13 {

using Int = unsigned long long;

constexpr auto extra_v2 = Int{10000000000000};

struct IntDiff {
  IntDiff(const Int lhs, const Int rhs) {
    negative = rhs > lhs;
    abs = negative ? rhs - lhs : lhs - rhs;
  }
  bool negative;
  Int abs;
};

class Button {
public:
  Button(const QString &button) {
    auto rx = QRegExp("^Button (?:A|B): X\\+(\\d+), Y\\+(\\d+)$");
    const auto valid = rx.exactMatch(button);
    if (not valid) {
      common::throwInvalidArgumentError(
          QString("puzzle_2024_13::Button: cannot parse string \"%1\"")
              .arg(button));
    }
    m_x = rx.cap(1).toULongLong();
    m_y = rx.cap(2).toULongLong();
  }

  Int x() const { return m_x; }
  Int y() const { return m_y; }

private:
  Int m_x;
  Int m_y;
};

class Prize {
public:
  Prize(const QString &prize, bool v2) {
    auto rx = QRegExp("^Prize: X=(\\d+), Y=(\\d+)$");
    const auto valid = rx.exactMatch(prize);
    if (not valid) {
      common::throwInvalidArgumentError(
          QString("puzzle_2024_13::Prize: cannot parse string \"%1\"")
              .arg(prize));
    }
    m_x = rx.cap(1).toULongLong();
    m_y = rx.cap(2).toULongLong();
    if (v2) {
      m_x += extra_v2;
      m_y += extra_v2;
    }
  }

  Int x() const { return m_x; }
  Int y() const { return m_y; }

private:
  Int m_x;
  Int m_y;
};

struct Solution {
  Solution(Int a, Int b) : nb_a_pushes{a}, nb_b_pushes{b} {}
  Int nb_a_pushes;
  Int nb_b_pushes;
};

class ClawMachine {
public:
  ClawMachine(const QString &button_A, const QString &button_B,
              const QString &price, bool v2)
      : m_button_A{button_A}, m_button_B{button_B}, m_prize{price, v2} {}

  std::optional<Solution> solve() const {
    const auto determinant = IntDiff(m_button_A.x() * m_button_B.y(),
                                     m_button_A.y() * m_button_B.x());
    if (determinant.abs == Int{0}) {
      return std::nullopt;
    }

    auto sol_a =
        IntDiff(m_button_B.y() * m_prize.x(), m_button_B.x() * m_prize.y());
    if (sol_a.negative != determinant.negative or
        sol_a.abs % determinant.abs != Int{0}) {
      return std::nullopt;
    }

    auto sol_b =
        IntDiff(m_button_A.x() * m_prize.y(), m_button_A.y() * m_prize.x());
    if (sol_b.negative != determinant.negative or
        sol_b.abs % determinant.abs != Int{0}) {
      return std::nullopt;
    }

    return Solution(sol_a.abs / determinant.abs, sol_b.abs / determinant.abs);
  }

private:
  Button m_button_A;
  Button m_button_B;
  Prize m_prize;
};

class ClawMachines {
public:
  ClawMachines(const QString &input, bool v2 = false) {
    const auto lines = common::splitLines(input, true);
    if (lines.size() % 3 != 0) {
      common::throwInvalidArgumentError(
          QString("puzzle_2024_13::ClawMachines: number of lines in input (%1) "
                  "is not a multiple of 3")
              .arg(lines.size()));
    }
    m_machines.reserve(lines.size() / 3);
    for (auto i = 0; i + 2 < lines.size(); i += 3) {
      m_machines.emplace_back(lines[i], lines[i + 1], lines[i + 2], v2);
    }
  }

  QString solve() const {
    auto result = Int{0};
    for (const auto &machine : m_machines) {
      const auto solution = machine.solve();
      if (solution.has_value()) {
        result += Int{3} * solution->nb_a_pushes + solution->nb_b_pushes;
      }
    }
    return QString("%1").arg(result);
  }

private:
  std::vector<ClawMachine> m_machines;
};

} // namespace puzzle_2024_13

void Solver_2024_13_1::solve(const QString &input) {
  const auto machines = puzzle_2024_13::ClawMachines(input);
  emit finished(machines.solve());
}

void Solver_2024_13_2::solve(const QString &input) {
  const auto machines = puzzle_2024_13::ClawMachines(input, true);
  emit finished(machines.solve());
}
