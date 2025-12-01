#include <solvers/common.h>
#include <solvers/2017/puzzle_2017_05.h>

namespace puzzle_2017_05 {

class Instructions {
public:
  Instructions(const QString &input)
      : m_intructions{common::toVecInt(input, '\n')} {}

  QString solve(bool v2) const {
    auto copy = m_intructions;
    auto current = 0;
    auto nb_steps = 0u;
    while (current >= 0 and current < m_intructions.size()) {
      const auto next = current + copy[current];
      if (v2 and copy[current] > 2) {
        --copy[current];
      } else {
        ++copy[current];
      }
      current = next;
      ++nb_steps;
    }
    return QString("%1").arg(nb_steps);
  }

private:
  QVector<int> m_intructions;
};

} // namespace puzzle_2017_05

void Solver_2017_05_1::solve(const QString &input) {
  const auto instructions = puzzle_2017_05::Instructions(input);
  emit finished(instructions.solve(false));
}

void Solver_2017_05_2::solve(const QString &input) {
  const auto instructions = puzzle_2017_05::Instructions(input);
  emit finished(instructions.solve(true));
}
