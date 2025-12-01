#include <solvers/common.h>
#include <solvers/2019/intcodecomputer.h>
#include <solvers/solvers.h>

namespace event_2019 {

Int State::get(const Int address, const QChar & /*mode*/) const {
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

QVector<Int> Instruction::getParameters(const QString &parameters_modes,
                                        const State &state) {
  auto it_mode = parameters_modes.crbegin();
  const auto get_next_mode = [&it_mode, &parameters_modes]() {
    if (it_mode == parameters_modes.crend()) {
      return QChar('0');
    }
    const auto mode = *it_mode;
    ++it_mode;
    return mode;
  };
  auto parameters = QVector<Int>();
  parameters.reserve(m_nb_parameters);
  for (auto i = Int(1); i <= m_nb_parameters; ++i) {
    const auto mode = get_next_mode();
    parameters << state.get(state.pointer() + i, mode);
  }
  return parameters;
}

/******************************************************************************/

Addition::Addition() : Instruction{Int(3)} {}

void Addition::apply(const QString &parameters_modes, State &state) {
  const auto parameters = getParameters(parameters_modes, state);
  state.set(parameters[2], parameters[0] + parameters[1]);
  state.jump(Int(4));
}

/******************************************************************************/

Multiplication::Multiplication() : Instruction{Int(3)} {}

void Multiplication::apply(const QString &parameters_modes, State &state) {
  const auto parameters = getParameters(parameters_modes, state);
  state.set(parameters[2], parameters[0] * parameters[1]);
  state.jump(Int(4));
}

/******************************************************************************/

const auto instructions = QHash<QString, std::shared_ptr<Instruction>>{
    {"01", std::make_shared<Addition>()},
    {"02", std::make_shared<Multiplication>()}};

bool applyCurrent(State &state) {
  auto parameters_modes =
      QString("%1").arg(state.get(state.pointer(), '1'), 2, 10, QChar('0'));
  const auto opcode = parameters_modes.mid(parameters_modes.size() - 2);
  if (opcode == "99") {
    return false;
  }
  const auto it = instructions.constFind(opcode);
  if (it == instructions.cend()) {
    common::throwInvalidArgumentError(
        QString("event_2019::appyCurrent: unrecognized opcode \"%1\"")
            .arg(opcode));
  }
  parameters_modes.chop(2);
  (*it)->apply(parameters_modes, state);
  return true;
}

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
  while (applyCurrent(m_state)) {
  }
}

} // namespace event_2019
