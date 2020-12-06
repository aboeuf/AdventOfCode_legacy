#include <2019/puzzle_2019_5.h>
#include <2019/intcodecomputer.h>
#include <common.h>

void Solver_2019_5_1::solve(const QString& input) const
{
  event_2019::IntcodeComputer computer(common::toIntValues(input), {1});
  computer.run();
  if (computer.outputs().isEmpty())
    emit finished("FAILURE");
  else
    emit finished(QString::number(computer.outputs().back()));
}

void Solver_2019_5_2::solve(const QString& input) const
{
  event_2019::IntcodeComputer computer(common::toIntValues(input), {5});
  computer.run();
  if (computer.outputs().isEmpty())
    emit finished("FAILURE");
  else
    emit finished(QString::number(computer.outputs().back()));
}
