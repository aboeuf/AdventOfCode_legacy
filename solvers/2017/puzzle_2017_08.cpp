#include <solvers/common.h>
#include <limits>
#include <solvers/2017/puzzle_2017_08.h>

namespace puzzle_2017_08 {

enum class IterateOperator { increase, decrease };

const auto iterate_operators = QHash<QString, IterateOperator>(
    {{"inc", IterateOperator::increase}, {"dec", IterateOperator::decrease}});

inline IterateOperator stringToIterateOperator(const QString &str) {
  const auto it = iterate_operators.constFind(str);
  if (it == iterate_operators.cend()) {
    common::throwInvalidArgumentError(
        QString(
            "puzzle_2017_08::stringToIterateOperator: unknown operator \"%1\"")
            .arg(str));
  }
  return it.value();
}

enum class ConditionOperator {
  equals,
  not_equals,
  greater_than,
  lesser_than,
  greater_than_or_equal_to,
  lesser_than_or_equal_to
};

const auto condition_operators = QHash<QString, ConditionOperator>(
    {{"==", ConditionOperator::equals},
     {"!=", ConditionOperator::not_equals},
     {">", ConditionOperator::greater_than},
     {"<", ConditionOperator::lesser_than},
     {">=", ConditionOperator::greater_than_or_equal_to},
     {"<=", ConditionOperator::lesser_than_or_equal_to}});

inline ConditionOperator stringToConditionOperator(const QString &str) {
  const auto it = condition_operators.constFind(str);
  if (it == condition_operators.cend()) {
    common::throwInvalidArgumentError(
        QString("puzzle_2017_08::stringToConditionOperator: unknown operator "
                "\"%1\"")
            .arg(str));
  }
  return it.value();
}

inline int toInt(const QString &sign, const QString &abs_value) {
  auto ok = true;
  auto value = abs_value.toInt(&ok);
  if (not ok) {
    common::throwInvalidArgumentError(
        QString("puzzle_2017_08::toInt: cannot convert input string "
                "\"%1\" to integer")
            .arg(abs_value));
  }
  return sign == "-" ? -value : value;
}

class Instruction {
public:
  Instruction(const QString &input) {
    auto rx = QRegExp("^([a-z]+) (dec|inc) (-?)(\\d+) if ([a-z]+) "
                      "(==|!=|<|>|<=|>=) (-?)(\\d+)$");
    if (not rx.exactMatch(input)) {
      common::throwInvalidArgumentError(
          QString(
              "puzzle_2017_08::Instruction: cannot parse input string \"%1\"")
              .arg(input));
    }
    m_iterate_register = rx.cap(1);
    m_iterate_operator = stringToIterateOperator(rx.cap(2));
    m_iterate_operand = toInt(rx.cap(3), rx.cap(4));
    m_condition_register = rx.cap(5);
    m_condition_operator = stringToConditionOperator(rx.cap(6));
    m_condition_operand = toInt(rx.cap(7), rx.cap(8));
  }

  const QString &iterateRegister() const { return m_iterate_register; }
  IterateOperator iterateOperator() const { return m_iterate_operator; }
  int iterateOperand() const { return m_iterate_operand; }
  const QString &conditionRegister() const { return m_condition_register; }
  ConditionOperator conditionOperator() const { return m_condition_operator; }
  int conditionOperand() const { return m_condition_operand; }

private:
  QString m_iterate_register;
  IterateOperator m_iterate_operator;
  int m_iterate_operand;
  QString m_condition_register;
  ConditionOperator m_condition_operator;
  int m_condition_operand;
};

class Registers {
public:
  int max() const {
    auto max = std::numeric_limits<int>::min();
    for (auto it = std::cbegin(m_data); it != std::cend(m_data); ++it) {
      max = std::max(max, it.value());
    }
    return max;
  }

  int get(const QString &key) const {
    const auto it = m_data.constFind(key);
    if (it == m_data.cend()) {
      common::throwInvalidArgumentError(
          QString("puzzle_2017_08::Registers: cannot find register \"%1\"")
              .arg(key));
    }
    return it.value();
  }

  void set(const QString &key, int value) { m_data[key] = value; }

private:
  QHash<QString, int> m_data;
};

class RegisterInstructions {
public:
  RegisterInstructions(const QString &input) {
    const auto lines = common::splitLines(input, true);
    m_instructions.reserve(lines.size());
    for (const auto &line : lines) {
      m_instructions.emplace_back(line);
      m_registers.set(m_instructions.back().iterateRegister(), 0);
      m_registers.set(m_instructions.back().conditionRegister(), 0);
    }
  }

  QString solveOne() {
    applyAll();
    return QString("%1").arg(m_registers.max());
  }

  QString solveTwo() {
    applyAll();
    return QString("%1").arg(m_max);
  }

private:
  bool matchesCondition(const Instruction &instruction) const {
    const auto value = m_registers.get(instruction.conditionRegister());
    const auto operand = instruction.conditionOperand();
    switch (instruction.conditionOperator()) {
    case ConditionOperator::equals:
      return value == operand;
    case ConditionOperator::not_equals:
      return value != operand;
    case ConditionOperator::greater_than:
      return value > operand;
    case ConditionOperator::lesser_than:
      return value < operand;
    case ConditionOperator::greater_than_or_equal_to:
      return value >= operand;
    case ConditionOperator::lesser_than_or_equal_to:
      return value <= operand;
    }
    common::throwRunTimeError("puzzle_2017_08::RegisterInstructions::apply: "
                              "unknown condition operator");
    return false;
  }

  void apply(const Instruction &instruction) {
    if (matchesCondition(instruction)) {
      const auto &label = instruction.iterateRegister();
      const auto value = m_registers.get(label);
      const auto operand = instruction.iterateOperand();
      if (instruction.iterateOperator() == IterateOperator::increase) {
        m_registers.set(label, value + operand);
      } else if (instruction.iterateOperator() == IterateOperator::decrease) {
        m_registers.set(label, value - operand);
      } else {
        common::throwRunTimeError("puzzle_2017_08::RegisterInstructions::apply:"
                                  " unknown iterate operator");
      }
      m_max = std::max(m_max, m_registers.get(label));
    }
  }

  void applyAll() {
    for (const auto &instruction : m_instructions) {
      apply(instruction);
    }
  }

  std::vector<Instruction> m_instructions;
  Registers m_registers;
  int m_max = std::numeric_limits<int>::min();
};

} // namespace puzzle_2017_08

void Solver_2017_08_1::solve(const QString &input) {
  auto instructions = puzzle_2017_08::RegisterInstructions(input);
  emit finished(instructions.solveOne());
}

void Solver_2017_08_2::solve(const QString &input) {
  auto instructions = puzzle_2017_08::RegisterInstructions(input);
  emit finished(instructions.solveTwo());
}
