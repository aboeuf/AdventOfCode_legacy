#include <deque>
#include <solvers/2021/puzzle_2021_11.h>
#include <solvers/common.h>

struct GridCoordinates {
  GridCoordinates(int row, int column) : i{row}, j{column} {}
  int i{0};
  int j{0};
};

class Octopuses {
public:
  Octopuses(const QString &input) {
    const auto lines = common::splitLines(input);
    m_grid.reserve(lines.size());
    for (const auto &line : lines) {
      m_grid.emplace_back();
      m_grid.back().reserve(line.size());
      for (const auto &c : line)
        m_grid.back().push_back(QString(c).toUInt());
      m_nb_octopuses += static_cast<uint>(m_grid.back().size());
    }
  }

  QString solvePuzzleOne() {
    auto nb_flashes = 0u;
    for (auto i = 0; i < 100; ++i)
      nb_flashes += step();
    return QString("%1").arg(nb_flashes);
  }

  QString solvePuzzleTwo() {
    for (auto i = 0u;; ++i) {
      const auto nb_flashes = step();
      if (nb_flashes == m_nb_octopuses)
        return QString("%1").arg(i + 1);
    }
    return "error";
  }

private:
  uint step() {
    auto to_be_flashed = std::deque<GridCoordinates>{};
    for (auto i = 0; i < static_cast<int>(m_grid.size()); ++i) {
      for (auto j = 0; j < static_cast<int>(m_grid[i].size()); ++j) {
        ++m_grid[i][j];
        if (m_grid[i][j] >= 10u)
          to_be_flashed.emplace_back(i, j);
      }
    }
    auto nb_flashes = 0u;
    while (not to_be_flashed.empty()) {
      flash(to_be_flashed.front(), to_be_flashed);
      to_be_flashed.pop_front();
      ++nb_flashes;
    }
    for (auto &row : m_grid)
      for (auto &octopus : row)
        if (octopus >= 10u)
          octopus = 0u;
    return nb_flashes;
  }

  void flash(const GridCoordinates &c,
             std::deque<GridCoordinates> &to_be_flashed) {
    for (auto di = -1; di <= 1; ++di) {
      const auto row = c.i + di;
      if (row >= 0 and row < static_cast<int>(m_grid.size())) {
        for (auto dj = -1; dj <= 1; ++dj) {
          const auto col = c.j + dj;
          if (col >= 0 and col < static_cast<int>(m_grid[row].size())) {
            if (m_grid[row][col] == 9u)
              to_be_flashed.emplace_back(row, col);
            ++m_grid[row][col];
          }
        }
      }
    }
  }

  std::vector<std::vector<uint>> m_grid;
  uint m_nb_octopuses{0u};
};

void Solver_2021_11_1::solve(const QString &input) {
  emit finished(Octopuses(input).solvePuzzleOne());
}

void Solver_2021_11_2::solve(const QString &input) {
  emit finished(Octopuses(input).solvePuzzleTwo());
}
