#include <2019/puzzle_2019_2.h>
#include <common.h>
#include <QDebug>
#include <array>

namespace intcode_computer {

constexpr auto HALT_CODE = 99;
constexpr auto OP_SIZE = 4;
constexpr auto NB_OPERANDS = OP_SIZE - 1;

using Array = std::array<int, NB_OPERANDS>;

enum ReturnCode
{
  OK,
  HALT,
  ERROR
};

ReturnCode read(const std::vector<int>& program, int position, int& result)
{
  if (position < 0 || position >= static_cast<int>(program.size())) {
    qWarning() << "intcode_computer::read: out of range position:" << position << ", program size =" << program.size();
    return ERROR;
  }
  result = program[position];
  return OK;
}

ReturnCode write(std::vector<int>& program, int position, int value)
{
  if (position < 0 || position >= static_cast<int>(program.size())) {
    qWarning() << "intcode_computer::write: out of range position:" << position << ", program size =" << program.size();
    return ERROR;
  }
  program[position] = value;
  return OK;
}

ReturnCode readOperands(const std::vector<int>& program, int position, Array& operands)
{
  for (int i = 0; i < NB_OPERANDS; ++i) {
    if (read(program, position + i + 1, operands[i]) == ERROR) {
      qWarning() << "intcode_computer::readOperands: bad index" << i;
      return ERROR;
    }
  }
  return OK;
}

ReturnCode readValues(const std::vector<int>& program, const Array& operands, Array& values)
{
  for (int i = 0; i < NB_OPERANDS; ++i) {
    if (read(program, operands[i], values[i]) == ERROR) {
      qWarning() << "intcode_computer::readValue: bad index" << i;
      return ERROR;
    }
  }
  return OK;
}

ReturnCode addition(std::vector<int>& program, int& position, int val_0, int val_1, int res_pos) {
  position += OP_SIZE;
  return write(program, res_pos, val_0 + val_1);
}

ReturnCode multiplication(std::vector<int>& program, int& position, int val_0, int val_1, int res_pos) {
  position += OP_SIZE;
  return write(program, res_pos, val_0 * val_1);
}

ReturnCode apply(std::vector<int>& program, int& position)
{
  int op_code;
  if (read(program, position, op_code) == ERROR)
    return ERROR;
  if (op_code == HALT_CODE)
    return HALT;
  Array op;
  if (readOperands(program, position, op) == ERROR)
    return ERROR;
  Array val;
  if (readValues(program, op, val) == ERROR)
    return ERROR;
  if (op_code == 1)
    return addition(program, position, val[0], val[1], op[2]);
  if (op_code == 2)
    return multiplication(program, position, val[0], val[1], op[2]);
  return ERROR;
}

bool run(std::vector<int>& progam)
{
  int position = 0;
  ReturnCode status;
  do { status = apply(progam, position); } while (status == OK);
  return status == HALT;
}

}

PuzzleSolver Puzzle_2019_2::solver_1 = [](const QString& input) {
  auto program = common::toIntValues(input);
  if (program.size() > 1)
    program[1] = 12;
  if (program.size() > 2)
    program[2] = 2;
  if (intcode_computer::run(program))
    return QString::number(program[0]);
  return QString{"FAILURE"};
};

PuzzleSolver Puzzle_2019_2::solver_2 = [](const QString& input) {
  const auto program = common::toIntValues(input);
  if (program.size() < 3)
    return QString{"BAD SIZE"};
  for (int noun = 0; noun < 100; ++noun) {
    for (int verb = 0; verb < 100; ++verb) {
      auto prog = program;
      prog[1] = noun;
      prog[2] = verb;
      if (intcode_computer::run(prog) && prog[0] == 19690720)
        return QString::number(100 * noun + verb);
    }
  }
  return QString{"FAILURE"};
};

