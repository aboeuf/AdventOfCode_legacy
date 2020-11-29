#ifndef SOLVERS_H
#define SOLVERS_H

#include <unordered_map>
#include <functional>
#include <QString>

class Solvers
{
public:
  using Solver = std::function<QString(const QString&)>;

  Solvers();
  QString operator () (int year, int day, const QString& input) const;

private:
  std::unordered_map<int, std::unordered_map<int, Solver>> m_solvers;
};

#endif // SOLVERS_H
