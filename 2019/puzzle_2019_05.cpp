#include <2019/puzzle_2019_05.h>
#include <2019/intcodecomputer.h>
#include <common.h>

#include <QDebug>

void Solver_2019_05_1::solve(const QString& input)
{
  event_2019::IntcodeComputer computer(common::toLongLongIntValues(input), {1});
  computer.run();
  if (computer.outputs().isEmpty())
    emit finished("FAILURE");
  else
    emit finished(QString::number(computer.outputs().back()));
}

void Solver_2019_05_2::solve(const QString& input)
{
  event_2019::IntcodeComputer computer(common::toLongLongIntValues(input), {5});
  computer.run();
  if (computer.outputs().isEmpty())
    emit finished("FAILURE");
  else
    emit finished(QString::number(computer.outputs().back()));
}
