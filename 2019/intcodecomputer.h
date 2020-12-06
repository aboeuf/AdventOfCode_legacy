#ifndef INTCODECOMPUTER_H
#define INTCODECOMPUTER_H

#include <QObject>
#include <QMap>
#include <vector>
#include <array>

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
    NO_INPUT
  };

  IntcodeComputer(const std::vector<int>& initial_memory, const QList<int>& inputs = {});

  void run();
  Status status() const;
  const QList<int>& outputs() const;
  bool readAt(int address, int& value, bool imediate_mode = true);
  bool writeAt(int address, int value,  bool imediate_mode = false);

signals:
  void askForInput(const QString& type);
  void sendOutput(const QString& output);
  void stop();

//public slots:
//  void onInputReceived(const QString& input);

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
  std::vector<int> m_memory;
  Status m_status = VALID;
  QList<int> m_inputs;
  QList<int> m_outputs;
};

}

#endif // INTCODECOMPUTER_H
