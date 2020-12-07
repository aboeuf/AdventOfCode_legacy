#include <solvers.h>

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
  m_solvers[2019][5][1] = new Solver_2019_5_2();

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

  for (auto year : m_solvers.values())
    for (auto day : year.values())
      for (auto solver : day.values())
        connect(solver, SIGNAL(finished(QString)), this, SLOT(onSolved(QString)));
}

Solvers::~Solvers()
{
  for (auto year : m_solvers.values())
    for (auto day : year.values())
      for (auto solver : day.values())
        delete solver;
}

void Solvers::operator()(const QString& input, int year, int day, int puzzle) const
{
  if (m_solvers.contains(year))
    if (m_solvers[year].contains(day))
      if (m_solvers[year][day].contains(puzzle)) {
        m_solvers[year][day][puzzle]->solve(input);
        return;
      }
  emit finished("Not implemented");
}

void Solvers::onSolved(const QString& output) const
{
  emit finished(output);
}
