#pragma once
#include <solvers.h>

namespace puzzle_2019_08
{

struct Layer : public QVector<QVector<uint>>
{
  Layer();
  uint nbOccurences(int value) const;
  void debug() const;
  QString toString() const;
  uint m_index{0};
};

}

class Solver_2019_08_1 : public Solver
{
public:
  void solve(const QString& input) override;
};

class Solver_2019_08_2 : public Solver
{
public:
  void solve(const QString& input) override;
};

