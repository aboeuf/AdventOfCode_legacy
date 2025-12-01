#include <solvers/2016/puzzle_2016_02.h>
#include <solvers/common.h>

namespace puzzle_2016_02 {

class Keypad {
public:
  Keypad(QStringList &&buttons) : m_buttons{buttons} {}

  std::optional<QChar> get(const QPoint &position) const {
    if (position.x() >= 0 and position.x() < m_buttons.size()) {
      const auto &row = m_buttons[position.x()];
      if (position.y() >= 0 and position.y() < row.size() and
          row[position.y()] != 'X') {
        return row[position.y()];
      }
    }
    return std::nullopt;
  }

  QPoint followInstruction(const QString &instruction,
                           const QPoint &starting_position) const {
    auto current_position = starting_position;
    for (const auto &c : instruction) {
      auto next_position = current_position;
      if (c == 'U') {
        --next_position.rx();
      } else if (c == 'D') {
        ++next_position.rx();
      } else if (c == 'L') {
        --next_position.ry();
      } else if (c == 'R') {
        ++next_position.ry();
      }
      if (get(next_position).has_value()) {
        std::swap(current_position, next_position);
      }
    }
    return current_position;
  }

private:
  QStringList m_buttons;
};

class Intruction {
public:
  Intruction(const QString &input)
      : m_instructions{common::splitLines(input)} {}

  QString solve(const Keypad &keypad, const QPoint &starting_position) const {
    auto result = QString();
    auto current_position = starting_position;
    for (const auto &instruction : m_instructions) {
      current_position =
          keypad.followInstruction(instruction, current_position);
      const auto c = keypad.get(current_position);
      if (c.has_value()) {
        result.push_back(*c);
      }
    }
    return result;
  }

  QString solveOne() const {
    auto buttons = QStringList{};
    buttons << "123" << "456" << "789";
    return solve(Keypad(std::move(buttons)), QPoint(1, 1));
  }

  QString solveTwo() const {
    auto buttons = QStringList{};
    buttons << "XX1XX" << "X234X" << "56789" << "XABCX" << "XXDXX";
    return solve(Keypad(std::move(buttons)), QPoint(2, 0));
  }

private:
  QStringList m_instructions;
};

} // namespace puzzle_2016_02

void Solver_2016_02_1::solve(const QString &input) {
  const auto instructions = puzzle_2016_02::Intruction{input};
  emit finished(instructions.solveOne());
}

void Solver_2016_02_2::solve(const QString &input) {
  const auto instructions = puzzle_2016_02::Intruction{input};
  emit finished(instructions.solveTwo());
}
