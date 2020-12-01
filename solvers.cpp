#include <solvers.h>
#include <2019/puzzle_2019_1.h>
#include <2019/puzzle_2019_2.h>
#include <2020/puzzle_2020_1.h>

Solvers::Solvers()
{
  m_solvers[2019][1] = Puzzle_2019_1::solver;
  m_solvers[2019][2] = Puzzle_2019_2::solver;
  m_solvers[2020][1] = Puzzle_2020_1::solver;
}

QString Solvers::operator () (int year, int day, const QString& input) const
{
  const auto year_it = m_solvers.find(year);
  if (year_it != m_solvers.end()) {
    const auto day_it = year_it->second.find(day);
    if (day_it != year_it->second.end())
      return day_it->second(input);
  }
  return QString{"Not implemented"};
}
