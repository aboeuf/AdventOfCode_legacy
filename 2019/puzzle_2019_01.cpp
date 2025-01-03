#include <2019/puzzle_2019_01.h>
#include <common.h>

namespace puzzle_2019_01 {

using Int = unsigned long long;

inline Int computeFuel(const Int mass, bool recursive) {
  auto fuel = mass / Int(3);
  if (fuel < Int(3)) {
    return Int(0);
  }
  fuel -= Int(2);
  if (recursive) {
    fuel += computeFuel(fuel, true);
  }
  return fuel;
}

class ModulesMasses {
public:
  ModulesMasses(const QString &input)
      : m_masses{common::toVecULongLong(input, '\n')} {}

  QString solve(bool v2) const {
    auto sum = Int(0);
    for (const auto mass : m_masses) {
      sum += computeFuel(mass, v2);
    }
    return QString("%1").arg(sum);
  }

private:
  QVector<Int> m_masses;
};

} // namespace puzzle_2019_01

void Solver_2019_01_1::solve(const QString &input) {
  emit finished(puzzle_2019_01::ModulesMasses(input).solve(false));
}

void Solver_2019_01_2::solve(const QString &input) {
  emit finished(puzzle_2019_01::ModulesMasses(input).solve(true));
}
