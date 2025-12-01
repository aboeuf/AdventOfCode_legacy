#include <solvers/common.h>
#include <solvers/2022/puzzle_2022_04.h>

namespace puzzle_2022_04 {

using Int = uint;

class SectionAssignment {
public:
  SectionAssignment() = default;
  SectionAssignment(const QString &input) {
    const auto values = common::toVecUInt(input, '-');
    m_min = values[0];
    m_max = values[1];
  }

  Int min() const { return m_min; }
  Int max() const { return m_max; }

  bool contains(const SectionAssignment &other) const {
    return m_min <= other.min() and other.max() <= m_max;
  }

private:
  Int m_min{0};
  Int m_max{0};
};

class PairAssignement {
public:
  PairAssignement(const QString &input) {
    const auto pairs = common::splitValues(input);
    m_first = SectionAssignment{pairs[0]};
    m_second = SectionAssignment{pairs[1]};
  }

  const SectionAssignment &first() const { return m_first; }
  const SectionAssignment &second() const { return m_second; }

  bool isSelfContained() const {
    return m_first.contains(m_second) or m_second.contains(m_first);
  }

  bool isDisjoint() const {
    return m_first.max() < m_second.min() or m_second.max() < m_first.min();
  }

  bool isOverlapping() const { return not isDisjoint(); }

private:
  SectionAssignment m_first;
  SectionAssignment m_second;
};

class Assignements {
public:
  Assignements(const QString &input) {
    const auto lines = common::splitLines(input);
    for (const auto &line : lines)
      m_assignements.emplace_back(line);
  }

  Int nbSelfContained() const {
    auto sum = Int{0};
    for (const auto &pair : m_assignements)
      if (pair.isSelfContained())
        ++sum;
    return sum;
  }

  Int nbOverlapping() const {
    auto sum = Int{0};
    for (const auto &pair : m_assignements)
      if (pair.isOverlapping())
        ++sum;
    return sum;
  }

private:
  std::vector<PairAssignement> m_assignements;
};

} // namespace puzzle_2022_04

void Solver_2022_04_1::solve(const QString &input) {
  const auto assignements = puzzle_2022_04::Assignements{input};
  emit finished(QString("%1").arg(assignements.nbSelfContained()));
}

void Solver_2022_04_2::solve(const QString &input) {
  const auto assignements = puzzle_2022_04::Assignements{input};
  emit finished(QString("%1").arg(assignements.nbOverlapping()));
}
