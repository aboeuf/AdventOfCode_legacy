#ifndef INTCODECOMPUTER_H
#define INTCODECOMPUTER_H

#include <QObject>
#include <QMap>
#include <QVector>
#include <array>

namespace event_2019 {

using Int = long long int;

class IntcodeComputer : public QObject
{
  Q_OBJECT

public:  
  enum Status
  {
    HALT,
    VALID,
    BAD_OPCODE,
    BAD_ADDRESS,
    WAITING_FOR_INPUT
  };

  enum ParameterMode
  {
    POSITION,
    IMEDIATE,
    RELATIVE
  };

  IntcodeComputer(const QVector<Int>& initial_memory = {},
                  const QList<Int>& inputs = {});

  void reset(const QVector<Int>& initial_memory, const QList<Int>& inputs = {});
  void run();
  Status status() const;
  void operator << (Int input);
  void resetIO(Int input);
  const QList<Int>& outputs() const;
  bool readAt(Int address, Int& value, ParameterMode mode = IMEDIATE);
  bool writeAt(Int address, Int value,  ParameterMode mode = POSITION);

private:
  using Modes = std::array<ParameterMode, 3>;

  void addition(Modes& modes);
  void multiplication(Modes& modes);
  bool input();
  void output(Modes& modes);
  void jump_if_true(Modes& modes);
  void jump_if_false(Modes& modes);
  void less_than(Modes& modes);
  void equals(Modes& modes);
  void relative_base_offset(Modes& modes);

  Int m_instruction_pointer{0};
  QMap<Int, Int> m_memory{};
  Status m_status = VALID;
  QList<Int> m_inputs{};
  QList<Int> m_outputs{};
  Int m_relative_base{0};
};

}

#endif // INTCODECOMPUTER_H
