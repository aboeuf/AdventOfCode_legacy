#include <2024/puzzle_2024_11.h>
#include <common.h>

#include <deque>
#include <unordered_map>

namespace puzzle_2024_11 {

using Int = unsigned long long;

struct Stone {
  Stone(Int value) {
    if (value == Int{0}) {
      children.push_back(Int{1});
      return;
    }
    const auto str = QString("%1").arg(value);
    if (str.size() % 2 != 0) {
      children.push_back(Int{2024} * value);
      return;
    }
    const auto half = str.size() / 2;
    children.push_back(str.midRef(0, half).toULongLong());
    children.push_back(str.midRef(half).toULongLong());
  }

  std::deque<Int> children;
  std::unordered_map<Int, Int> nb_chidren_after_n_steps;
};

class Stones {
public:
  Stones(const QString &input)
      : m_initial_stones{common::toULongLong(input, ' ')} {
    for (const auto value : m_initial_stones) {
      addStone(value);
    }
  }

  Int getNbChidrenAfterNSteps(Int nb_steps) {
    auto sum = Int{0};
    for (const auto value : m_initial_stones) {
      sum += getNbChidrenAfterNSteps(value, nb_steps);
    }
    return sum;
  }

private:
  void addStone(Int value) {
    auto it = m_generated_stones.insert(std::make_pair(value, Stone(value)));
    if (it.second) {
      for (const auto child_value : it.first->second.children) {
        addStone(child_value);
      }
    }
  }

  Int getNbChidrenAfterNSteps(Int parent_value, Int nb_steps) {
    if (nb_steps == Int{0}) {
      return Int{1};
    }
    auto &parent_stone = m_generated_stones.find(parent_value)->second;
    const auto it = parent_stone.nb_chidren_after_n_steps.find(nb_steps);
    if (it != std::cend(parent_stone.nb_chidren_after_n_steps)) {
      return it->second;
    }
    auto &result = parent_stone.nb_chidren_after_n_steps[nb_steps];
    result = Int{0};
    for (const auto child_value : parent_stone.children) {
      result += getNbChidrenAfterNSteps(child_value, nb_steps - Int{1});
    }
    return result;
  }

  QVector<Int> m_initial_stones;
  std::unordered_map<Int, Stone> m_generated_stones;
};

} // namespace puzzle_2024_11

void Solver_2024_11_1::solve(const QString &input) {
  auto stones = puzzle_2024_11::Stones(input);
  emit finished(QString("%1").arg(stones.getNbChidrenAfterNSteps(25u)));
}

void Solver_2024_11_2::solve(const QString &input) {
  auto stones = puzzle_2024_11::Stones(input);
  emit finished(QString("%1").arg(stones.getNbChidrenAfterNSteps(75u)));
}
