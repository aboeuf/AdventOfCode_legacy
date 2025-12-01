#include <solvers/common.h>
#include <solvers/2022/puzzle_2022_01.h>

namespace puzzle_2022_01 {

using Int = long long unsigned int;

class Elf {
public:
  Elf() = default;

  void addDish(Int calories) {
    m_foods.push_back(calories);
    m_total_calories += calories;
  }

  Int totalCalories() const { return m_total_calories; }
  const std::vector<Int> &foods() const { return m_foods; }

private:
  Int m_total_calories{0};
  std::vector<Int> m_foods{};
};

class Elves {
public:
  Elves(const QString &input) {
    const auto lines = common::splitLines(input);
    m_elves.emplace_back();
    for (const auto &line : lines) {
      if (line.isEmpty())
        m_elves.emplace_back();
      else
        m_elves.back().addDish(line.toULongLong());
    }
    while (not m_elves.empty() and m_elves.back().foods().empty())
      m_elves.pop_back();
    std::sort(std::begin(m_elves), std::end(m_elves),
              [](const auto &lhs, const auto &rhs) {
                return lhs.totalCalories() > rhs.totalCalories();
              });
  }

  const std::vector<Elf> &elves() const { return m_elves; }

  Int maxCalories(std::size_t nb_elves) const {
    auto calories = Int{0};
    for (auto i = 0u; i < nb_elves; ++i)
      calories += m_elves[i].totalCalories();
    return calories;
  }

private:
  std::vector<Elf> m_elves{};
};

} // namespace puzzle_2022_01

void Solver_2022_01_1::solve(const QString &input) {
  const auto elves = puzzle_2022_01::Elves{input};
  emit finished(QString("%1").arg(elves.maxCalories(1)));
}

void Solver_2022_01_2::solve(const QString &input) {
  const auto elves = puzzle_2022_01::Elves{input};
  emit finished(QString("%1").arg(elves.maxCalories(3)));
}
