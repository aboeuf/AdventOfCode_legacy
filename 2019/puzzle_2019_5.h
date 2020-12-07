#pragma once
#include <solvers.h>

class Solver_2019_5_1 : public IncodeComputerUsingSolver
{
public:
  void solve(const QString& input) override;

public slots:
  void onComputerStopped() override;
};

class Solver_2019_5_2 : public IncodeComputerUsingSolver
{
public:
  void solve(const QString& input) override;

public slots:
  void onComputerStopped() override;
};

