#include <solvers.h>

#include <2019/puzzle_2019_1.h>
#include <2019/puzzle_2019_2.h>
#include <2019/puzzle_2019_3.h>

#include <2020/puzzle_2020_1.h>
#include <2020/puzzle_2020_2.h>
#include <2020/puzzle_2020_3.h>

const PuzzleSolver default_solver = [](const QString&) { return QString{"Not implemented"}; };

Solver::Solver(const PuzzleSolver& solver_1 = default_solver,
               const PuzzleSolver& solver_2 = default_solver) :
  m_solver_1{solver_1},
  m_solver_2{solver_2}
{
}

QString Solver::operator() (const QString& input, bool puzzle_1) const
{
  return puzzle_1 ? m_solver_1(input) : m_solver_2(input);
}

Solvers::Solvers()
{
  m_solvers[2019][1] = Solver{Puzzle_2019_1::solver_1, Puzzle_2019_1::solver_2};
  m_solvers[2019][2] = Solver{Puzzle_2019_2::solver_1, Puzzle_2019_2::solver_2};
  m_solvers[2019][3] = Solver{Puzzle_2019_3::solver_1, Puzzle_2019_3::solver_2};

  m_solvers[2020][1] = Solver{Puzzle_2020_1::solver_1, Puzzle_2020_1::solver_2};
  m_solvers[2020][2] = Solver{Puzzle_2020_2::solver_1, Puzzle_2020_2::solver_2};
  m_solvers[2020][3] = Solver{Puzzle_2020_3::solver_1, Puzzle_2020_3::solver_2};
}

QString Solvers::operator () (const QString& input, int year, int day, bool puzzle_1) const
{
  const auto year_it = m_solvers.find(year);
  if (year_it != m_solvers.end()) {
    const auto day_it = year_it->second.find(day);
    if (day_it != year_it->second.end()) {
      return day_it->second(input, puzzle_1);
    }
  }
  return QString{"Not implemented"};
}
