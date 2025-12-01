#include <solvers/2021/puzzle_2021_07.h>
#include <solvers/common.h>

class CostsComputer {
public:
  CostsComputer() { m_costs.push_back(0u); }

  uint getCost(uint translation) {
    const auto n = static_cast<uint>(m_costs.size());
    if (translation + 1 > n) {
      m_costs.resize(translation + 1);
      for (auto i = n; i <= translation; ++i)
        m_costs[i] = m_costs[i - 1] + i;
    }
    return m_costs[translation];
  }

private:
  std::vector<uint> m_costs;
};

class Crabs {
public:
  Crabs(const QString &input) {
    m_positions = common::toVecUInt(input);
    for (const auto &p : m_positions) {
      m_min = std::min(m_min, p);
      m_max = std::max(m_max, p);
    }
  }

  uint alignementCost_V1(uint position) const {
    auto cost = 0u;
    for (const auto &p : m_positions)
      cost += p < position ? position - p : p - position;
    return cost;
  }

  uint alignementCost_V2(uint position) const {
    auto cc = CostsComputer{};
    auto cost = 0u;
    for (const auto &p : m_positions)
      cost += cc.getCost(p < position ? position - p : p - position);
    return cost;
  }

  QString solvePuzzleOne() const {
    auto min_cost = std::numeric_limits<uint>::max();
    for (auto p = m_min; p <= m_max; ++p)
      min_cost = std::min(alignementCost_V1(p), min_cost);
    return QString{"%1"}.arg(min_cost);
  }

  QString solvePuzzleTwo() const {
    auto min_cost = std::numeric_limits<uint>::max();
    for (auto p = m_min; p <= m_max; ++p)
      min_cost = std::min(alignementCost_V2(p), min_cost);
    return QString{"%1"}.arg(min_cost);
  }

private:
  QVector<uint> m_positions;
  uint m_min{std::numeric_limits<uint>::max()};
  uint m_max{0u};
};

void Solver_2021_07_1::solve(const QString &input) {
  emit finished(Crabs{input}.solvePuzzleOne());
}

void Solver_2021_07_2::solve(const QString &input) {
  emit output(QString("%1").arg(CostsComputer().getCost(11u)));
  emit finished(Crabs{input}.solvePuzzleTwo());
}
