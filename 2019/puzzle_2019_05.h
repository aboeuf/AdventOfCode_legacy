#pragma once
#include <solvers.h>

class Solver_2019_05_1 : public IntcodeComputerUsingSolver
{
public:
  void solve(const QString& input) override;

public slots:
  void onComputerStopped() override;
};

class Solver_2019_05_2 : public IntcodeComputerUsingSolver
{
public:
  void solve(const QString& input) override;

public slots:
  void onComputerStopped() override;
};

