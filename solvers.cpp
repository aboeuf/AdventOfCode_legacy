#include <solvers.h>
#include <mainwindow.h>

#include <2019/puzzle_2019_1.h>
#include <2019/puzzle_2019_2.h>
#include <2019/puzzle_2019_3.h>
#include <2019/puzzle_2019_4.h>
#include <2019/puzzle_2019_5.h>

#include <2020/puzzle_2020_1.h>
#include <2020/puzzle_2020_2.h>
#include <2020/puzzle_2020_3.h>
#include <2020/puzzle_2020_4.h>
#include <2020/puzzle_2020_5.h>
#include <2020/puzzle_2020_6.h>
#include <2020/puzzle_2020_7.h>
#include <2020/puzzle_2020_8.h>
#include <2020/puzzle_2020_9.h>
#include <2020/puzzle_2020_10.h>
#include <2020/puzzle_2020_11.h>
#include <2020/puzzle_2020_12.h>

void Solver::onInputReceived(const QString&) {}

void IncodeComputerUsingSolver::onInputReceived(const QString& input)
{
  if (input.isEmpty()) {
    emit askInput("Intcode computer is asking for an integer input");
    return;
  }
  bool ok;
  int integer = input.toInt(&ok);
  if (!ok) {
    emit askInput("Intcode computer is asking for an integer input");
    return;
  }
  emit integerInputReceived(integer);
}

Solvers::Solvers()
{
  m_solvers[2019][1][1] = new Solver_2019_1_1();
  m_solvers[2019][1][2] = new Solver_2019_1_2();
  m_solvers[2019][2][1] = new Solver_2019_2_1();
  m_solvers[2019][2][2] = new Solver_2019_2_2();
  m_solvers[2019][3][1] = new Solver_2019_3_1();
  m_solvers[2019][3][2] = new Solver_2019_3_2();
  m_solvers[2019][4][1] = new Solver_2019_4_1();
  m_solvers[2019][4][2] = new Solver_2019_4_2();
  m_solvers[2019][5][1] = new Solver_2019_5_1();
  m_solvers[2019][5][2] = new Solver_2019_5_2();

  m_solvers[2020][1][1] = new Solver_2020_1_1();
  m_solvers[2020][1][2] = new Solver_2020_1_2();
  m_solvers[2020][2][1] = new Solver_2020_2_1();
  m_solvers[2020][2][2] = new Solver_2020_2_2();
  m_solvers[2020][3][1] = new Solver_2020_3_1();
  m_solvers[2020][3][2] = new Solver_2020_3_2();
  m_solvers[2020][4][1] = new Solver_2020_4_1();
  m_solvers[2020][4][2] = new Solver_2020_4_2();
  m_solvers[2020][5][1] = new Solver_2020_5_1();
  m_solvers[2020][5][2] = new Solver_2020_5_2();
  m_solvers[2020][6][1] = new Solver_2020_6_1();
  m_solvers[2020][6][2] = new Solver_2020_6_2();
  m_solvers[2020][7][1] = new Solver_2020_7_1();
  m_solvers[2020][7][2] = new Solver_2020_7_2();
  m_solvers[2020][8][1] = new Solver_2020_8_1();
  m_solvers[2020][8][2] = new Solver_2020_8_2();
  m_solvers[2020][9][1] = new Solver_2020_9_1();
  m_solvers[2020][9][2] = new Solver_2020_9_2();
  m_solvers[2020][10][1] = new Solver_2020_10_1();
  m_solvers[2020][10][2] = new Solver_2020_10_2();
  m_solvers[2020][11][1] = new Solver_2020_11_1();
  m_solvers[2020][11][2] = new Solver_2020_11_2();
  m_solvers[2020][12][1] = new Solver_2020_12_1();
  m_solvers[2020][12][2] = new Solver_2020_12_2();
}

Solvers::~Solvers()
{
  for (auto year : m_solvers.values())
    for (auto day : year.values())
      for (auto solver : day.values())
        delete solver;
}

Solver* Solvers::operator()(int year, int day, int puzzle) const
{
  if (m_solvers.contains(year))
    if (m_solvers[year].contains(day))
      if (m_solvers[year][day].contains(puzzle))
        return m_solvers[year][day][puzzle];
  return nullptr;
}

