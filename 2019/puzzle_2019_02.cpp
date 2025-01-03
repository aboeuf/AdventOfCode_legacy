#include <2019/intcodecomputer.h>
#include <2019/puzzle_2019_02.h>
#include <common.h>

namespace puzzle_2019_02 {

using namespace event_2019;

class GravityAssistProgram {
public:
  GravityAssistProgram(const QString &input)
      : m_computer{common::splitLines(input, true).front()} {}

  QString run(const Int first_input, const Int second_input) {
    m_computer.reset();
    m_computer.set(1, first_input);
    m_computer.set(2, second_input);
    m_computer.run();
    return QString("%1").arg(m_computer.state().get(0));
  }

  QString solve() {
    for (auto noun = Int(0); noun < Int(100); ++noun) {
      for (auto verb = Int(0); verb < Int(100); ++verb) {
        if (run(noun, verb) == "19690720") {
          return QString("%1").arg(Int(100) * noun + verb);
        }
      }
    }
    return "Failure";
  }

private:
  IntcodeComputer m_computer;
};

} // namespace puzzle_2019_02

void Solver_2019_02_1::solve(const QString &input) {
  emit finished(puzzle_2019_02::GravityAssistProgram(input).run(12, 2));
}

void Solver_2019_02_2::solve(const QString &input) {
  emit finished(puzzle_2019_02::GravityAssistProgram(input).solve());
}
