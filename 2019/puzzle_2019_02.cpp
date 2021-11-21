#include <2019/puzzle_2019_02.h>
#include <common.h>

void Solver_2019_02_1::solve(const QString& input)
{
  event_2019::IntcodeComputer computer(common::toLongLongIntValues(input));
  computer.writeAt(1, 12);
  computer.writeAt(2, 2);
  computer.run();
  long long int result;
  computer.readAt(0, result);
  if (computer.status() == event_2019::IntcodeComputer::HALT)
    emit finished(QString::number(result));
  else
    emit finished(QString{"FAILURE"});
}

void Solver_2019_02_2::solve(const QString& input)
{
  using Int = long long int;
  Int result;
  QVector<Int> program = common::toLongLongIntValues(input);
  event_2019::IntcodeComputer computer;
  for (Int noun = 0; noun < 100; ++noun) {
    for (Int verb = 0; verb < 100; ++verb) {
      computer.reset(program);
      computer.writeAt(1, noun);
      computer.writeAt(2, verb);
      computer.run();
      computer.readAt(0, result);
      if (computer.status() == event_2019::IntcodeComputer::HALT && result == 19690720) {
        emit finished(QString::number(100 * noun + verb));
        return;
      }
    }
  }
  emit finished(QString{"FAILURE"});
}

