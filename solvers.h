#ifndef SOLVERS_H
#define SOLVERS_H

#include <unordered_map>
#include <functional>
#include <optional>
#include <QString>

using PuzzleSolver = std::function<QString(const QString&)>;
using PuzzleSolverOpt = std::optional<PuzzleSolver>;
struct Solver : public std::pair<PuzzleSolverOpt, PuzzleSolverOpt>
{
  Solver();
  Solver(const PuzzleSolver& solver_1);
  Solver(const PuzzleSolver& solver_1, const PuzzleSolver& solver_2);
};

class Solvers
{
public:
  Solvers();
  QString operator () (int year, int day, bool puzzle_1, const QString& input) const;

private:
  std::unordered_map<int, std::unordered_map<int, Solver>> m_solvers;
};

#endif // SOLVERS_H
