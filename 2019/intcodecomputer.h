#ifndef INTCODECOMPUTER_H
#define INTCODECOMPUTER_H

#include <QObject>
#include <QMap>
#include <QVector>
#include <array>

class IntcodeComputerUsingSolver;

namespace event_2019 {

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

  IntcodeComputer() = default;
  IntcodeComputer(IntcodeComputerUsingSolver* solver,
                  const QVector<int>& initial_memory = {},
                  const QList<int>& inputs = {});

  void reset(const QVector<int>& initial_memory, const QList<int>& inputs = {});
  QList<int> run();
  Status status() const;
  void operator << (int input);
  void resetIO(int input);
  const QList<int>& outputs() const;
  bool readAt(int address, int& value, bool imediate_mode = true);
  bool writeAt(int address, int value,  bool imediate_mode = false);

signals:
  void finished();

public slots:
  void onInputReceived(int input);

private:
  using Modes = std::array<bool, 3>;

  void addition(Modes& modes);
  void multiplication(Modes& modes);
  bool input();
  void output(Modes& modes);
  void jump_if_true(Modes& modes);
  void jump_if_false(Modes& modes);
  void less_than(Modes& modes);
  void equals(Modes& modes);

  int m_instruction_pointer{0};
  QVector<int> m_memory{};
  Status m_status = VALID;
  QList<int> m_inputs{};
  QList<int> m_outputs{};
  IntcodeComputerUsingSolver* m_solver{nullptr};
};

}

#endif // INTCODECOMPUTER_H
