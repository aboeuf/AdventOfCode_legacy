#include <2019/puzzle_2019_2.h>
#include <common.h>

void Solver_2019_2_1::solve(const QString& input)
{
  delete m_computer;
  m_computer = new event_2019::IntcodeComputer(this, common::toIntValues(input));
  m_computer->writeAt(1, 12);
  m_computer->writeAt(2, 2);
  m_computer->run();
}

void Solver_2019_2_1::onComputerStopped()
{
  if (!m_computer) {
    emit finished(QString{"Error: m_computer is nullptr"});
    return;
  }
  int result;
  m_computer->readAt(0, result);
  if (m_computer->status() == event_2019::IntcodeComputer::HALT)
    emit finished(QString::number(result));
  else
    emit finished(QString{"FAILURE"});
  delete m_computer;
}

void Solver_2019_2_2::solve(const QString& input)
{
  m_noun = 0;
  m_verb = 0;
  m_program = common::toIntValues(input);
  delete m_computer;
  m_computer = new event_2019::IntcodeComputer(this, m_program);
  m_computer->writeAt(1, m_noun);
  m_computer->writeAt(2, m_verb);
  m_computer->run();
}

void Solver_2019_2_2::onComputerStopped()
{
  if (!m_computer) {
    emit finished(QString{"Error: m_computer is nullptr"});
    return;
  }
  int result;
  m_computer->readAt(0, result);
  if (m_computer->status() == event_2019::IntcodeComputer::HALT && result == 19690720) {
    delete m_computer;
    emit finished(QString::number(100 * m_noun + m_verb));
    return;
  }
  ++m_verb;
  if (m_verb > 100) {
    m_verb = 0;
    ++m_noun;
    if (m_noun > 100) {
      delete m_computer;
      emit finished(QString{"FAILURE"});
      return;
    }
  }
  delete m_computer;
  m_computer = new event_2019::IntcodeComputer(this, m_program);
  m_computer->writeAt(1, m_noun);
  m_computer->writeAt(2, m_verb);
  m_computer->run();
}

