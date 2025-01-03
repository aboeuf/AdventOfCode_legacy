#include <2019/intcodecomputer.h>
#include <common.h>
#include <solvers.h>

namespace event_2019 {

Int State::get(const Int address) const {
  const auto it = m_memory.constFind(address);
  if (it == m_memory.cend()) {
    common::throwInvalidArgumentError(
        QString("event_2019::State::get: bad memory address %1").arg(address));
  }
  return *it;
}

void State::set(const Int address, const Int value) {
  m_memory[address] = value;
}

Int State::pointer() const { return m_pointer; }

void State::jump(const Int distance) { m_pointer += distance; }

void State::goTo(const Int address) { m_pointer = address; }

void State::reset(const QVector<Int> &values) {
  m_pointer = Int(0);
  m_memory.clear();
  const auto size = static_cast<Int>(values.size());
  for (auto address = 0u; address < size; ++address) {
    m_memory[address] = values[address];
  }
}

/******************************************************************************/

Instruction::Instruction(const Int nb_parameters)
    : m_nb_parameters{nb_parameters} {}

QVector<Int> Instruction::getParameters(const State &state) {
  auto parameters = QVector<Int>();
  parameters.reserve(m_nb_parameters);
  for (auto i = Int(1); i <= m_nb_parameters; ++i) {
    parameters << state.get(state.pointer() + i);
  }
  return parameters;
}

/******************************************************************************/

Addition::Addition() : Instruction{Int(3)} {}

void Addition::apply(State &state) {
  const auto parameters = getParameters(state);
  const auto lhs = state.get(parameters[0]);
  const auto rhs = state.get(parameters[1]);
  state.set(parameters[2], lhs + rhs);
  state.jump(Int(4));
}

/******************************************************************************/

Multiplication::Multiplication() : Instruction{Int(3)} {}

void Multiplication::apply(State &state) {
  const auto parameters = getParameters(state);
  const auto lhs = state.get(parameters[0]);
  const auto rhs = state.get(parameters[1]);
  state.set(parameters[2], lhs * rhs);
  state.jump(Int(4));
}

/******************************************************************************/

const auto instructions = QHash<Int, std::shared_ptr<Instruction>>{
    {Int(1), std::make_shared<Addition>()},
    {Int(2), std::make_shared<Multiplication>()}};

/******************************************************************************/

IntcodeComputer::IntcodeComputer(const QString &input)
    : m_initial_memory{common::toVecULongLong(input, ',')} {
  reset();
}

const State &IntcodeComputer::state() const { return m_state; }

void IntcodeComputer::reset() { m_state.reset(m_initial_memory); }

void IntcodeComputer::set(const Int address, const Int value) {
  m_state.set(address, value);
}

void IntcodeComputer::run() {
  for (;;) {
    const auto opcode = m_state.get(m_state.pointer());
    if (opcode == Int(99)) {
      return;
    }
    const auto instruction = instructions.constFind(opcode);
    if (instruction == instructions.cend()) {
      common::throwRunTimeError(
          QString("event_2019::IntodeComputer: bad opcode %1").arg(opcode));
    }
    (*instruction)->apply(m_state);
  }
}

} // namespace event_2019
