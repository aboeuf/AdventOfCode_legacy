#include <2019/puzzle_2019_07.h>
#include <common.h>
#include <QVector>

namespace puzzle_2019_07
{

Amplifier::Amplifier(const Amplifier& other) : Amplifier{}
{
  m_phase_setting = other.m_phase_setting;
  m_program = other.m_program;
}

int Amplifier::run(int input, bool feedback_loop_mode)
{
  if (feedback_loop_mode)
    m_computer.resetIO(input);
  else
    m_computer.reset(m_program, {m_phase_setting, input});
  QList<int> outputs = m_computer.run();
  if (outputs.size() < 1)
    return 0;
  return outputs.front();
}

const event_2019::IntcodeComputer& Amplifier::computer() const { return m_computer; }

Amplifiers::Amplifiers(const QString& input)
{
  const QStringList lines = common::splitLines(input);
  QVector<int> program = lines.empty() ? QVector<int>{} : common::toIntValues(lines.front());
  m_amps.resize(5);
  for (int i = 0; i < 5; ++i)
    m_amps[i].m_program = program;
}

uint Amplifiers::getMaxOutput(QString& max_settings)
{
  uint max_output = 0;
  QVector<int> phases_settings = {0, 1, 2, 3, 4};
  do
  {
    for (int i = 0; i < 5; ++i)
      m_amps[i].m_phase_setting = phases_settings[i];
    int output = 0;
    QList<int> debug;
    for (int i = 0; i < 5; ++i) {
      output = m_amps[i].run(output);
      debug << output;
    }
    uint out = output < 0 ? 0 : static_cast<uint>(output);
    if (out > max_output) {
      max_output = out;
      max_settings.clear();
      for (int i = 0; i < 5; ++i)
        max_settings += QString::number(phases_settings[i]);
    }
  } while (std::next_permutation(phases_settings.begin(), phases_settings.end()));
  return max_output;
}

uint Amplifiers::getMaxOutputWithFeedbackLoop(QString& max_settings)
{
  uint max_output = 0;
  QVector<int> phases_settings = {5, 6, 7, 8, 9};
  do
  {
    for (int i = 0; i < 5; ++i)
      m_amps[i].m_phase_setting = phases_settings[i];
    int output = 0;
    for (int i = 0; i < 5; ++i)
      output = m_amps[i].run(output);
    while (m_amps.back().computer().status() != event_2019::IntcodeComputer::HALT)
      for (int i = 0; i < 5; ++i)
        output = m_amps[i].run(output, true);
    uint out = output < 0 ? 0 : static_cast<uint>(output);
    if (out > max_output) {
      max_output = out;
      max_settings.clear();
      for (int i = 0; i < 5; ++i)
        max_settings += QString::number(phases_settings[i]);
    }
  } while (std::next_permutation(phases_settings.begin(), phases_settings.end()));
  return max_output;
}

} // namepspace puzzle_2019_07

void Solver_2019_07_1::solve(const QString& input)
{
  using namespace puzzle_2019_07;
  Amplifiers amps{input};
  QString max_settings;
  uint max_output = amps.getMaxOutput(max_settings);
  emit output("Max settings: " + max_settings);
  emit finished(QString::number(max_output));
}

void Solver_2019_07_2::solve(const QString& input)
{
  using namespace puzzle_2019_07;
  Amplifiers amps{input};
  QString max_settings;
  uint max_output = amps.getMaxOutputWithFeedbackLoop(max_settings);
  emit output("Max settings: " + max_settings);
  emit finished(QString::number(max_output));
}

