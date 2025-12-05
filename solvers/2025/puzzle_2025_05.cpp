#include <solvers/2025/puzzle_2025_05.h>
#include <solvers/common.h>

namespace puzzle_2025_05 {

using Int = unsigned long long;

class FreshSet {
public:
  void extend(const QVector<Int> &values) {
    if (std::size(values) != 2 or values.back() < values.front()) {
      throw std::invalid_argument("ill-formed input");
    }
    auto it = m_map.emplace(values.front(), values.back()).first;
    it->second = std::max(it->second, values.back());
    for (auto it = std::begin(m_map); std::next(it) != std::end(m_map);) {
      if (std::next(it)->first <= it->second) {
        it->second = std::max(it->second, std::next(it)->second);
        it = std::prev(m_map.erase(std::next(it)));
      } else {
        ++it;
      }
    }
  }

  bool contains(Int value) const {
    return m_map.empty() or value < std::cbegin(m_map)->first
               ? false
               : value <= std::prev(m_map.upper_bound(value))->second;
  }

  Int nbFreshIngredients() const {
    auto sum = Int{0};
    for (const auto [start, end] : m_map) {
      sum += end - start + Int{1};
    }
    return sum;
  }

private:
  std::map<Int, Int> m_map;
};

class Inventory {
public:
  Inventory(const QString &input) {
    const auto lines = common::splitLines(input);
    auto nb_lines = lines.size();
    auto current_line = 0;
    for (const auto &line : lines) {
      const auto values = common::toVecULongLong(line, '-');
      if (values.size() == 2) {
        m_fresh_ingredients.extend(values);
      } else {
        if (m_available_ingredient.empty()) {
          m_available_ingredient.reserve(nb_lines - current_line);
        }
        m_available_ingredient.emplace_back(values.front());
      }
      ++current_line;
    }
  }

  Int nbFreshAvailableIngredients() const {
    auto nb_fresh = Int{0};
    for (auto ingredient : m_available_ingredient) {
      if (m_fresh_ingredients.contains(ingredient)) {
        ++nb_fresh;
      }
    }
    return nb_fresh;
  }

  Int nbFreshIngredients() const {
    return m_fresh_ingredients.nbFreshIngredients();
  }

private:
  FreshSet m_fresh_ingredients;
  std::vector<Int> m_available_ingredient;
};

} // namespace puzzle_2025_05

void Solver_2025_05_1::solve(const QString &input) {
  const auto inventory = puzzle_2025_05::Inventory(input);
  emit finished(QString("%1").arg(inventory.nbFreshAvailableIngredients()));
}

void Solver_2025_05_2::solve(const QString &input) {
  const auto inventory = puzzle_2025_05::Inventory(input);
  emit finished(QString("%1").arg(inventory.nbFreshIngredients()));
}
