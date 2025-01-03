#include <2024/puzzle_2024_05.h>
#include <common.h>
#include <unordered_set>

namespace puzzle_2024_05 {

using Update = QVector<uint>;

inline uint getMidValue(const Update &update) {
  return update[update.size() / 2];
}

class Updates {
public:
  Updates(const QString &input) {
    const auto lines = common::splitLines(input);
    auto nb_remaining_lines = lines.size();
    auto memory_reserved = false;
    for (const auto &line : lines) {
      if (line.isEmpty()) {
        continue;
      }
      if (line.contains('|')) {
        const auto values = common::toVecUInt(line, '|');
        m_successors[values[0]].insert(values[1]);
      } else {
        if (not memory_reserved) {
          m_updates.reserve(nb_remaining_lines);
          memory_reserved = true;
        }
        m_updates.emplace_back(common::toVecUInt(line));
      }
      --nb_remaining_lines;
    }
  }

  QString solveOne() const {
    auto sum = uint{0};
    for (auto i = 0u; i < m_updates.size(); ++i) {
      if (isWellOrdered(i)) {
        sum += getMidValue(m_updates[i]);
      }
    }
    return QString("%1").arg(sum);
  }

  QString solveTwo() const {
    auto sum = uint{0};
    for (auto i = 0u; i < m_updates.size(); ++i) {
      if (not isWellOrdered(i)) {
        const auto ordered = sorted(i);
        sum += getMidValue(ordered);
      }
    }
    return QString("%1").arg(sum);
  }

private:
  bool areInCorrectOrder(uint lhs, uint rhs) const {
    const auto it = m_successors.find(rhs);
    if (it == std::cend(m_successors)) {
      return true;
    }
    return it->second.find(lhs) == std::cend(it->second);
  }

  bool isWellOrdered(std::size_t update_index) const {
    if (update_index >= m_updates.size()) {
      return false;
    }
    const auto &update = m_updates[update_index];
    for (auto i = 0; i < update.size(); ++i) {
      for (auto j = i + 1; j < update.size(); ++j) {
        if (not areInCorrectOrder(update[i], update[j])) {
          return false;
        }
      }
    }
    return true;
  }

  Update sorted(std::size_t update_index) const {
    if (update_index >= m_updates.size()) {
      return Update{};
    }
    auto copy = m_updates[update_index];
    std::sort(std::begin(copy), std::end(copy),
              [this](const auto &lhs, const auto &rhs) {
                return areInCorrectOrder(lhs, rhs);
              });
    return copy;
  }

  std::unordered_map<uint, std::unordered_set<uint>> m_successors;
  std::vector<Update> m_updates;
};

} // namespace puzzle_2024_05

void Solver_2024_05_1::solve(const QString &input) {
  const auto rules_and_update = puzzle_2024_05::Updates{input};
  emit finished(rules_and_update.solveOne());
}

void Solver_2024_05_2::solve(const QString &input) {
  const auto rules_and_update = puzzle_2024_05::Updates{input};
  emit finished(rules_and_update.solveTwo());
}
