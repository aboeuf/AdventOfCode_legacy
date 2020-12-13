#include <2019/puzzle_2019_05.h>
#include <2019/intcodecomputer.h>
#include <common.h>

#include <QDebug>

void Solver_2019_05_1::solve(const QString& input)
{
  delete m_computer;
  m_computer = new event_2019::IntcodeComputer(this, common::toIntValues(input), {1});
  m_computer->run();
}

void Solver_2019_05_1::onComputerStopped()
{
  if (!m_computer) {
    emit finished(QString{"Error: m_computer is nullptr"});
    return;
  }
  if (m_computer->outputs().isEmpty())
    emit finished("FAILURE");
  else
    emit finished(QString::number(m_computer->outputs().back()));
  delete m_computer;
}

void Solver_2019_05_2::solve(const QString& input)
{
  delete m_computer;
  m_computer = new event_2019::IntcodeComputer(this, common::toIntValues(input), {5});
  m_computer->run();
}

void Solver_2019_05_2::onComputerStopped()
{
  if (!m_computer) {
    emit finished(QString{"Error: m_computer is nullptr"});
    return;
  }
  if (m_computer->outputs().isEmpty())
    emit finished("FAILURE");
  else
    emit finished(QString::number(m_computer->outputs().back()));
  delete m_computer;
}
