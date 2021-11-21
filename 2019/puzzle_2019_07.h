#pragma once
#include <solvers.h>
#include <2019/intcodecomputer.h>

namespace puzzle_2019_07
{

using Int = long long int;

class Amplifier
{
public:
  Amplifier() = default;
  Amplifier(const Amplifier& other);

  Int m_phase_setting{0};
  QVector<Int> m_program{};

  Int run(Int input, bool feedback_loop_mode = false);
  const event_2019::IntcodeComputer& computer() const;

private:
  event_2019::IntcodeComputer m_computer{};
};

class Amplifiers
{
public:
  Amplifiers(const QString& input);

  uint getMaxOutput(QString& max_settings);
  uint getMaxOutputWithFeedbackLoop(QString& max_settings);

private:
  QVector<Amplifier> m_amps{};
};

}

class Solver_2019_07_1 : public Solver
{
public:
  void solve(const QString& input) override;
};

class Solver_2019_07_2 : public Solver
{
public:
  void solve(const QString& input) override;
};

