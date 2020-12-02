#include <solvers.h>

#include <2019/puzzle_2019_1.h>
#include <2019/puzzle_2019_2.h>
#include <2019/puzzle_2019_3.h>

#include <2020/puzzle_2020_1.h>
#include <2020/puzzle_2020_2.h>

Solver::Solver() :
  std::pair<PuzzleSolverOpt, PuzzleSolverOpt>{std::nullopt, std::nullopt} {}

Solver::Solver(const PuzzleSolver& solver_1) :
  std::pair<PuzzleSolverOpt, PuzzleSolverOpt>{solver_1, std::nullopt} {}

Solver::Solver(const PuzzleSolver& solver_1, const PuzzleSolver& solver_2) :
  std::pair<PuzzleSolverOpt, PuzzleSolverOpt>{solver_1, solver_2} {}

Solvers::Solvers()
{
  m_solvers[2019][1] = Solver{Puzzle_2019_1::solver};
  m_solvers[2019][2] = Solver{Puzzle_2019_2::solver};
  m_solvers[2019][3] = Solver{Puzzle_2019_3::solver_1};

  m_solvers[2020][1] = Solver{Puzzle_2020_1::solver};
  m_solvers[2020][2] = Solver{Puzzle_2020_2::solver};
}

QString Solvers::operator () (int year, int day, bool puzzle_1, const QString& input) const
{
  const auto year_it = m_solvers.find(year);
  if (year_it != m_solvers.end()) {
    const auto day_it = year_it->second.find(day);
    if (day_it != year_it->second.end()) {
      if (puzzle_1) {
        if (day_it->second.first)
          return (*(day_it->second.first))(input);
      } else {
        if (day_it->second.second)
          return (*(day_it->second.second))(input);
      }
    }
  }
  return QString{"Not implemented"};
}
