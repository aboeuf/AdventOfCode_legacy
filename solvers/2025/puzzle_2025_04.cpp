#include <solvers/2025/puzzle_2025_04.h>
#include <solvers/common.h>

#include <deque>

namespace puzzle_2025_04 {

class Grid {
public:
  Grid(const QString &input) {
    const auto lines = common::splitLines(input);
    for (auto i = 0; i < lines.size(); ++i) {
      for (auto j = 0; j < lines[i].size(); ++j) {
        if (lines[i][j] == QChar('@')) {
          m_grid[i][j] = 0;
        }
      }
    }
    for (const auto &[i, row] : m_grid) {
      for (const auto &[j, _] : row) {
        addToNeighbors(i, j, 1);
      }
    }
  }

  std::deque<std::pair<int, int>> getRemovableRolls() const {
    auto removable_rolls = std::deque<std::pair<int, int>>{};
    for (const auto &[i, row] : m_grid) {
      for (const auto &[j, nb_neighbors] : row) {
        if (nb_neighbors < 4) {
          removable_rolls.emplace_back(i, j);
        }
      }
    }
    return removable_rolls;
  }

  void addNeighbor(int i, int j, int increment) {
    auto it_i = m_grid.find(i);
    if (it_i != m_grid.end()) {
      auto it_j = it_i->second.find(j);
      if (it_j != it_i->second.end()) {
        it_j->second += increment;
      }
    }
  }

  void addToNeighbors(int i, int j, int increment) {
    for (auto ni = i - 1; ni < i + 2; ++ni) {
      for (auto nj = j - 1; nj < j + 2; ++nj) {
        if (i != ni or j != nj) {
          addNeighbor(ni, nj, increment);
        }
      }
    }
  }

  void removeRoll(int i, int j) {
    auto it_i = m_grid.find(i);
    if (it_i != m_grid.end()) {
      auto it_j = it_i->second.find(j);
      if (it_j != it_i->second.end()) {
        addToNeighbors(i, j, -1);
        it_i->second.erase(it_j);
        if (it_i->second.empty()) {
          m_grid.erase(it_i);
        }
      }
    }
  }

  bool removeRolls(uint &nb_removed) {
    const auto removable_rolls = getRemovableRolls();
    for (const auto &[i, j] : removable_rolls) {
      removeRoll(i, j);
    }
    nb_removed += std::size(removable_rolls);
    return not removable_rolls.empty();
  }

  uint removeAllRolls() {
    auto total_removed = 0u;
    while (removeRolls(total_removed)) {
    }
    return total_removed;
  }

private:
  int m_length;
  int m_width;
  std::unordered_map<int, std::unordered_map<int, int>> m_grid;
};

} // namespace puzzle_2025_04

void Solver_2025_04_1::solve(const QString &input) {
  const auto grid = puzzle_2025_04::Grid(input);
  emit finished(QString("%1").arg(std::size(grid.getRemovableRolls())));
}

void Solver_2025_04_2::solve(const QString &input) {
  auto grid = puzzle_2025_04::Grid(input);
  emit finished(QString("%1").arg(grid.removeAllRolls()));
}
