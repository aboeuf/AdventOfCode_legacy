#include "intcodecomputer.h"
#include <solvers.h>

namespace event_2019 {

IntcodeComputer::IntcodeComputer(IncodeComputerUsingSolver* solver,
                                 const QVector<int> &initial_memory,
                                 const QList<int>& inputs) :
  m_memory(initial_memory),
  m_inputs{inputs},
  m_solver{solver}
{
  connect(m_solver, SIGNAL(integerInputReceived(int)), this, SLOT(onInputReceived(int)));
  connect(this, SIGNAL(finished()), m_solver, SLOT(onComputerStopped()));
}

void IntcodeComputer::run()
{
  int raw_code;
  while (m_status == VALID) {
    if (readAt(m_instruction_pointer, raw_code)) {
      QString raw_code_str = QString("%1").arg(raw_code, 5, 10, QChar('0'));
      int opcode = raw_code_str.mid(3, 2).toInt();
      Modes modes = {raw_code_str[2] != '0',
                     raw_code_str[1] != '0',
                     raw_code_str[0] != '0'};
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
      case 99:
        m_status = HALT;
        break;
      default:
        m_status = BAD_OPCODE;
      }
    }
  }
  emit finished();
}

IntcodeComputer::Status IntcodeComputer::status() const
{
  return m_status;
}

void IntcodeComputer::operator << (int input) {
  m_inputs << input;
}

const QList<int>& IntcodeComputer::outputs() const
{
  return m_outputs;
}

bool IntcodeComputer::readAt(int address, int& value, bool imediate_mode)
{
  if (address < 0 || address >= static_cast<int>(m_memory.size())) {
    m_status = BAD_ADDRESS;
    return false;
  }
  value = m_memory[address];
  if (imediate_mode)
    return true;
  return readAt(value, value, true);
}

bool IntcodeComputer::writeAt(int address, int value, bool imediate_mode)
{
  if (imediate_mode)
    if (!readAt(address, address))
      return false;
  if (address < 0 || address >= static_cast<int>(m_memory.size())) {
    m_status = BAD_ADDRESS;
    return false;
  }
  m_memory[address] = value;
  return true;
}

void IntcodeComputer::onInputReceived(int input)
{
  m_inputs << input;
  m_status = VALID;
  run();
}

void IntcodeComputer::addition(Modes &modes) {
  int a, b, address;
  if (!readAt(m_instruction_pointer + 1, a, modes[0]))
    return;
  if (!readAt(m_instruction_pointer + 2, b, modes[1]))
    return;
  if (!readAt(m_instruction_pointer + 3, address))
    return;
  writeAt(address, a + b, modes[2]);
  m_instruction_pointer += 4;
}

void IntcodeComputer::multiplication(Modes &modes) {
  int a, b, address;
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
    emit m_solver->askInput("Intcode computer is asking for an integer as input");
    return false;
  }
  int value = m_inputs.front();
  m_inputs.pop_front();
  int address;
  if (readAt(m_instruction_pointer + 1, address))
    writeAt(address, value);
  m_instruction_pointer += 2;
  return true;
}

void IntcodeComputer::output(Modes& modes)
{
  int value;
  if (readAt(m_instruction_pointer + 1, value, modes[0])) {
    m_outputs << value;
    emit m_solver->output(QString::number(value));
  }
  m_instruction_pointer += 2;
}

void IntcodeComputer::jump_if_true(Modes& modes)
{
  int test_value, jump_address;
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
  int test_value, jump_address;
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
  int a, b, address;
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
  int a, b, address;
  if (!readAt(m_instruction_pointer + 1, a, modes[0]))
    return;
  if (!readAt(m_instruction_pointer + 2, b, modes[1]))
    return;
  if (!readAt(m_instruction_pointer + 3, address))
    return;
  writeAt(address, a == b ? 1 : 0, modes[2]);
  m_instruction_pointer += 4;
}

}
