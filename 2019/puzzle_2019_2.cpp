#include <2019/puzzle_2019_2.h>
#include <2019/intcodecomputer.h>
#include <common.h>

void Solver_2019_2_1::solve(const QString& input) const
{
  event_2019::IntcodeComputer computer(common::toIntValues(input));
  computer.writeAt(1, 12);
  computer.writeAt(2, 2);
  computer.run();
  int result;
  computer.readAt(0, result);
  if (computer.status() == event_2019::IntcodeComputer::HALT)
    emit finished(QString::number(result));
  else
    emit finished(QString{"FAILURE"});
}

void Solver_2019_2_2::solve(const QString& input) const
{
  const auto program = common::toIntValues(input);
  for (int noun = 0; noun < 100; ++noun) {
    for (int verb = 0; verb < 100; ++verb) {
      event_2019::IntcodeComputer computer(program);
      computer.writeAt(1, noun);
      computer.writeAt(2, verb);
      computer.run();
      int result;
      computer.readAt(0, result);
      if (computer.status() == event_2019::IntcodeComputer::HALT && result == 19690720) {
        emit finished(QString::number(100 * noun + verb));
        return;
      }
    }
  }
  emit finished(QString{"FAILURE"});
}

