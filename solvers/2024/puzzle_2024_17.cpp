#include <solvers/common.h>
#include <solvers/2024/puzzle_2024_17.h>

#include <iostream>

namespace puzzle_2024_17 {

using Int = unsigned long long;

constexpr auto nb_instructions = Int{8};
constexpr auto operand_is_combo = std::array<bool, nb_instructions>{
    true, false, true, false, false, true, true, true};

template <typename Container>
inline QString containerToString(const Container &c) {
  if (c.isEmpty()) {
    return QString("");
  }
  auto result = QString("%1").arg(c.front());
  for (auto i = 1; i < c.size(); ++i) {
    result += QString(",%1").arg(c.at(i));
  }
  return result;
}

template <typename FirstContainer, typename SecondContainer>
inline bool endsWith(const FirstContainer &first,
                     const SecondContainer &second) {
  const auto size_difference = first.size() - second.size();
  if (size_difference < 0) {
    return false;
  }
  for (auto i = 0; i < second.size(); ++i) {
    if (first.at(size_difference + i) != second.at(i)) {
      return false;
    }
  }
  return true;
}

template <typename FirstContainer, typename SecondContainer>
inline bool equals(const FirstContainer &first, const SecondContainer &second) {
  if (first.size() != second.size()) {
    return false;
  }
  for (auto i = 0; i < first.size(); ++i) {
    if (first.at(i) != second.at(i)) {
      return false;
    }
  }
  return true;
}

struct Instruction {
  Int opcode;
  Int operand;
};

class Computer {
private:
  QVector<Int> m_registers;
  QVector<Int> m_program;

public:
  Computer(const QString &input) {
    m_registers.resize(3);
    const auto lines = common::splitLines(input, true);
    if (lines.size() != 4) {
      common::throwInvalidArgumentError(
          QString("puzzle_2024_17::Computer: bad number of lines in input: %1 "
                  "instead of for")
              .arg(lines.size()));
    }
    const auto register_labels = QString("ABC");
    for (auto i = 0; i < 3; ++i) {
      const auto rx_str =
          QString("^Register %1: (\\d+)$").arg(register_labels[i]);
      auto rx = QRegExp(rx_str);
      if (not rx.exactMatch(lines[i])) {
        common::throwInvalidArgumentError(
            QString("puzzle_2024_17::Computer: cannot parse register %1 from "
                    "input \"%2\"")
                .arg(register_labels[i])
                .arg(lines[i]));
      }
      auto ok = true;
      m_registers[i] = rx.cap(1).toULongLong(&ok);
      if (not ok) {
        common::throwInvalidArgumentError(
            QString("puzzle_2024_17::Computer: cannot convert the value of "
                    "register %1 from string \"%2\" to unsigned integer")
                .arg(register_labels[i])
                .arg(rx.cap(1)));
      }
    }
    const auto tokens = common::splitValues(lines[3], ' ');
    if (tokens.size() != 2 or tokens[0] != "Program:") {
      common::throwInvalidArgumentError(
          QString("puzzle_2024_17::Computer: cannot parse program from input "
                  "\"%1\"")
              .arg(lines[3]));
    }
    try {
      m_program = common::toVecULongLong(tokens[1]);
    } catch (const std::exception &e) {
      common::throwInvalidArgumentError(
          QString("puzzle_2024_17::Computer: cannot convert program value from "
                  "input \"%1\"")
              .arg(e.what()));
    }
    if (m_program.size() % 2 != 0) {
      common::throwInvalidArgumentError(
          "puzzle_2024_17::Computer: program size is not even");
    }
  }

  QString solveOne() const { return containerToString(runProgram()); }

  QString solveTwo() const {
    auto initial_A_value = Int{0};
    while (not checkValue(initial_A_value, true)) {
      auto found = false;
      for (auto i = Int{0}; not found and i < Int{8}; ++i) {
        auto test_value = Int{8} * initial_A_value + i;
        if (checkValue(test_value)) {
          initial_A_value = test_value;
          found = true;
        }
      }
      if (not found) {
        ++initial_A_value;
      }
    }
    return QString("%1").arg(initial_A_value);
  }

private:
  Instruction getInstruction(const int position,
                             const QVector<Int> &registers) const {
    if (position % 2 != 0) {
      common::throwInvalidArgumentError(
          "puzzle_2024_17::Computer::getInstruction: position is not even");
    }
    if (position + 1 >= m_program.size()) {
      common::throwInvalidArgumentError(
          QString("puzzle_2024_17::Computer::getInstruction: out of range "
                  "position %1 "
                  "(program size is %2)")
              .arg(position + 1)
              .arg(m_program.size()));
    }
    auto instruction = Instruction();
    instruction.opcode = m_program[position];
    if (instruction.opcode >= nb_instructions) {
      common::throwInvalidArgumentError(
          QString("puzzle_2024_17::Computer::getInstruction: invalid opcode %1")
              .arg(instruction.opcode));
    }
    instruction.operand = m_program[position + Int{1}];
    if (operand_is_combo[instruction.opcode]) {
      if (Int{3} < instruction.operand and instruction.operand < Int{7}) {
        instruction.operand = registers[instruction.operand - Int{4}];
      } else if (instruction.operand > Int{6}) {
        common::throwInvalidArgumentError(
            QString("puzzle_2024_17::Computer::getInstruction: invalid combo "
                    "operand %1")
                .arg(instruction.operand));
      }
    }
    return instruction;
  }

  QList<Int> runProgram(
      const std::optional<Int> &initial_A_register = std::nullopt) const {
    auto registers = m_registers;
    if (initial_A_register.has_value()) {
      registers[0] = *initial_A_register;
    }
    auto output = QList<Int>{};
    auto current_position = 0;
    while (current_position < m_program.size()) {
      auto increment_positon = true;
      const auto instruction = getInstruction(current_position, registers);
      switch (instruction.opcode) {
      case Int{0}:
        registers[0] = registers[0] >> instruction.operand;
        break;
      case Int{1}:
        registers[1] = registers[1] ^ instruction.operand;
        break;
      case Int{2}:
        registers[1] = instruction.operand % Int{8};
        break;
      case Int{3}:
        if (registers[0] != Int{0}) {
          increment_positon = false;
          current_position = instruction.operand;
        }
        break;
      case Int{4}:
        registers[1] = registers[1] ^ registers[2];
        break;
      case Int{5}:
        output << instruction.operand % Int{8};
        break;
      case Int{6}:
        registers[1] = registers[0] >> instruction.operand;
        break;
      case Int{7}:
        registers[2] = registers[0] >> instruction.operand;
        break;
      }
      if (increment_positon) {
        current_position += Int{2};
      }
    }
    return output;
  }

  bool checkValue(const Int value, bool equality = false) const {
    const auto output = runProgram(value);
    return equality ? equals(m_program, output) : endsWith(m_program, output);
  }
};

} // namespace puzzle_2024_17

void Solver_2024_17_1::solve(const QString &input) {
  const auto computer = puzzle_2024_17::Computer(input);
  emit finished(computer.solveOne());
}

void Solver_2024_17_2::solve(const QString &input) {
  const auto computer = puzzle_2024_17::Computer(input);
  emit finished(computer.solveTwo());
}
