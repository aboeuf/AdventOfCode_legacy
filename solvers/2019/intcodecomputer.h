#pragma once

#include <QHash>
#include <QString>

namespace event_2019 {

using Int = unsigned long long;
using Memory = QHash<Int, Int>;

/******************************************************************************/

class State {
public:
  Int get(Int address, const QChar &) const;
  void set(Int address, Int value);

  Int pointer() const;
  void jump(Int distance);
  void goTo(Int address);

  void reset(const QVector<Int> &values);

private:
  Int m_pointer{0};
  Memory m_memory{};
};

/******************************************************************************/

class Instruction {
public:
  Instruction(Int nb_parameters);
  virtual ~Instruction() = default;
  QVector<Int> getParameters(const QString &parameters_modes,
                             const State &state);
  virtual void apply(const QString &parameters_modes, State &state) = 0;

private:
  Int m_nb_parameters;
};

/******************************************************************************/

class Addition : public Instruction {
public:
  Addition();
  void apply(const QString &parameters_modes, State &state) final;
};

/******************************************************************************/

class Multiplication : public Instruction {
public:
  Multiplication();
  void apply(const QString &parameters_modes, State &state) final;
};

/******************************************************************************/

class IntcodeComputer {
public:
  IntcodeComputer(const QString &input);

  const State &state() const;

  void reset();
  void set(Int address, Int value);
  void run();

private:
  QVector<Int> m_initial_memory;
  State m_state;
};

} // namespace event_2019
