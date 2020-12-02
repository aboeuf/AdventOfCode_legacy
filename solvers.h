#ifndef SOLVERS_H
#define SOLVERS_H

#include <unordered_map>
#include <functional>
#include <QString>

using PuzzleSolver = std::function<QString(const QString&)>;

class Solver
{
public:
  Solver(const PuzzleSolver& solver_1, const PuzzleSolver& solver_2);
  QString operator() (const QString& input, bool puzzle_1) const;

private:
  PuzzleSolver m_solver_1;
  PuzzleSolver m_solver_2;
};

class Solvers
{
public:
  Solvers();
  QString operator () (const QString& input, int year, int day, bool puzzle_1) const;

private:
  std::unordered_map<int, std::unordered_map<int, Solver>> m_solvers;
};

#endif // SOLVERS_H
