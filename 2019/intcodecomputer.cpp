#include "intcodecomputer.h"
#include <solvers.h>

namespace event_2019 {

IntcodeComputer::IntcodeComputer(const QVector<Int> &initial_memory,
                                 const QList<Int>& inputs) :
  m_inputs{inputs}
{
  for (Int i = 0; i < static_cast<Int>(initial_memory.size()); ++i)
    m_memory[i] = initial_memory[i];
}

void IntcodeComputer::reset(const QVector<Int>& initial_memory,
                            const QList<Int>& inputs)
{
  m_memory.clear();
  for (Int i = 0; i < static_cast<Int>(initial_memory.size()); ++i)
    m_memory[i] = initial_memory[i];
  m_inputs = inputs;
  m_status = VALID;
  m_instruction_pointer = 0;
  m_outputs.clear();
  m_relative_base = 0;
}

void IntcodeComputer::run()
{
  Int raw_code;
  while (m_status == VALID) {
    if (readAt(m_instruction_pointer, raw_code)) {

      QString raw_code_str = QString("%1").arg(raw_code, 5, 10, QChar('0'));
      Int opcode = raw_code_str.mid(3, 2).toLongLong();
      Modes modes = {static_cast<ParameterMode>(QString(raw_code_str[2]).toInt()),
                     static_cast<ParameterMode>(QString(raw_code_str[1]).toInt()),
                     static_cast<ParameterMode>(QString(raw_code_str[0]).toInt())};
      switch (opcode) {
      case 1:
        addition(modes);
        break;
      case 2:
        multiplication(modes);
        break;
      case 3:
        if (!input())
          return;
        break;
      case 4:
        output(modes);
        break;
      case 5:
        jump_if_true(modes);
        break;
      case 6:
        jump_if_false(modes);
        break;
      case 7:
        less_than(modes);
        break;
      case 8:
        equals(modes);
        break;
      case 9:
        relative_base_offset(modes);
        break;
      case 99:
        m_status = HALT;
        break;
      default:
        m_status = BAD_OPCODE;
      }
    }
  }
}

IntcodeComputer::Status IntcodeComputer::status() const
{
  return m_status;
}

void IntcodeComputer::operator << (Int input) {
  m_inputs << input;
}

void IntcodeComputer::resetIO(Int input) {
  m_status = VALID;
  m_inputs << input;
  m_outputs.clear();
}

const QList<Int>& IntcodeComputer::outputs() const
{
  return m_outputs;
}

bool IntcodeComputer::readAt(Int address, Int& value, ParameterMode mode)
{
  if (!m_memory.contains(address)) {
    if (address < 0) {
      m_status = BAD_ADDRESS;
      return false;
    }
    m_memory[address] = 0;
  }
  value = m_memory[address];
  if (mode == IMEDIATE)
    return true;
  if (mode == RELATIVE)
    value += m_relative_base;
  return readAt(value, value, IMEDIATE);
}

bool IntcodeComputer::writeAt(Int address, Int value, ParameterMode mode)
{
  if (mode == RELATIVE)
    return writeAt(m_relative_base + address, value, POSITION);
  if (mode == IMEDIATE)
    if (!readAt(address, address))
      return false;
  m_memory[address] = value;
  return true;
}

void IntcodeComputer::addition(Modes& modes) {
  Int a, b, address;
  if (!readAt(m_instruction_pointer + 1, a, modes[0]))
    return;
  if (!readAt(m_instruction_pointer + 2, b, modes[1]))
    return;
  if (!readAt(m_instruction_pointer + 3, address))
    return;
  writeAt(address, a + b, modes[2]);
  m_instruction_pointer += 4;
}

void IntcodeComputer::multiplication(Modes& modes) {
  Int a, b, address;
  if (!readAt(m_instruction_pointer + 1, a, modes[0]))
    return;
  if (!readAt(m_instruction_pointer + 2, b, modes[1]))
    return;
  if (!readAt(m_instruction_pointer + 3, address))
    return;
  writeAt(address, a * b, modes[2]);
  m_instruction_pointer += 4;
}

bool IntcodeComputer::input()
{
  if (m_inputs.isEmpty()) {
    m_status = WAITING_FOR_INPUT;
    return false;
  }
  Int value = m_inputs.front();
  m_inputs.pop_front();
  Int address;
  if (readAt(m_instruction_pointer + 1, address))
    writeAt(address, value);
  m_instruction_pointer += 2;
  return true;
}

void IntcodeComputer::output(Modes& modes)
{
  Int value;
  if (readAt(m_instruction_pointer + 1, value, modes[0]))
    m_outputs << value;
  m_instruction_pointer += 2;
}

void IntcodeComputer::jump_if_true(Modes& modes)
{
  Int test_value, jump_address;
  if (!readAt(m_instruction_pointer + 1, test_value, modes[0]))
    return;
  if (!readAt(m_instruction_pointer + 2, jump_address, modes[1]))
    return;
  if (test_value == 0)
    m_instruction_pointer += 3;
  else
    m_instruction_pointer = jump_address;
}

void IntcodeComputer::jump_if_false(Modes& modes)
{
  Int test_value, jump_address;
  if (!readAt(m_instruction_pointer + 1, test_value, modes[0]))
    return;
  if (!readAt(m_instruction_pointer + 2, jump_address, modes[1]))
    return;
  if (test_value == 0)
    m_instruction_pointer = jump_address;
  else
    m_instruction_pointer += 3;
}

void IntcodeComputer::less_than(Modes& modes)
{
  Int a, b, address;
  if (!readAt(m_instruction_pointer + 1, a, modes[0]))
    return;
  if (!readAt(m_instruction_pointer + 2, b, modes[1]))
    return;
  if (!readAt(m_instruction_pointer + 3, address))
    return;
  writeAt(address, a < b ? 1 : 0, modes[2]);
  m_instruction_pointer += 4;
}

void IntcodeComputer::equals(Modes& modes)
{
  Int a, b, address;
  if (!readAt(m_instruction_pointer + 1, a, modes[0]))
    return;
  if (!readAt(m_instruction_pointer + 2, b, modes[1]))
    return;
  if (!readAt(m_instruction_pointer + 3, address))
    return;
  writeAt(address, a == b ? 1 : 0, modes[2]);
  m_instruction_pointer += 4;
}

void IntcodeComputer::relative_base_offset(Modes& modes)
{
  Int value;
  if (!readAt(m_instruction_pointer + 1, value, modes[0]))
    return;
  m_relative_base += value;
  m_instruction_pointer += 2;
}

}
