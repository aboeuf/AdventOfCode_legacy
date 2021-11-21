#include <2019/puzzle_2019_09.h>
#include <2019/intcodecomputer.h>
#include <common.h>
#include <QDebug>

namespace puzzle_2019_09
{

} // namepspace puzzle_2019_09

void Solver_2019_09_1::solve(const QString& input)
{
  using namespace puzzle_2019_09;
  event_2019::IntcodeComputer computer(common::toLongLongIntValues(input), {1});
  computer.run();
  for (event_2019::Int out : computer.outputs())
    emit output(QString::number(out));
  if (computer.outputs().size() == 1)
    emit finished(QString::number(computer.outputs().front()));
  else
    emit finished("BOOST dianogsitic failure");
}

void Solver_2019_09_2::solve(const QString& input)
{
  using namespace puzzle_2019_09;
  emit finished(input);
}

