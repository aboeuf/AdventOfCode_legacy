#pragma once
#include <solvers.h>

class Solver_2019_02_1 : public IntcodeComputerUsingSolver
{
public:
  void solve(const QString& input) override;

public slots:
  void onComputerStopped() override;
};

class Solver_2019_02_2 : public IntcodeComputerUsingSolver
{
public:
  void solve(const QString& input) override;

public slots:
  void onComputerStopped() override;

private:
  QVector<int> m_program{};
  int m_noun{0};
  int m_verb{0};
};
