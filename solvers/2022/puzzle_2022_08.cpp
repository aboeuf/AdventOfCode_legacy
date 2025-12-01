#include <solvers/2022/puzzle_2022_08.h>
#include <solvers/common.h>

namespace puzzle_2022_08 {

struct Visibility {
  bool visible{false};
  uint distance{1};
};

inline Visibility rightVisibility(const std::vector<std::vector<uint>> &grid,
                                  const std::size_t i, const std::size_t j) {
  auto res = Visibility{};
  for (; j + res.distance < grid[i].size(); ++res.distance)
    if (grid[i][j + res.distance] >= grid[i][j])
      return res;
  --res.distance;
  res.visible = true;
  return res;
}

inline Visibility leftVisibility(const std::vector<std::vector<uint>> &grid,
                                 const std::size_t i, const std::size_t j) {
  if (j == 0u)
    return Visibility{true, 0};
  auto res = Visibility{};
  for (; res.distance <= j; ++res.distance)
    if (grid[i][j - res.distance] >= grid[i][j])
      return res;
  --res.distance;
  res.visible = true;
  return res;
}

inline Visibility bottomVisibility(const std::vector<std::vector<uint>> &grid,
                                   const std::size_t i, const std::size_t j) {
  auto res = Visibility{};
  for (; i + res.distance < grid.size(); ++res.distance)
    if (grid[i + res.distance][j] >= grid[i][j])
      return res;
  --res.distance;
  res.visible = true;
  return res;
}

inline Visibility topVisibility(const std::vector<std::vector<uint>> &grid,
                                const std::size_t i, const std::size_t j) {
  if (i == 0u)
    return Visibility{true, 0};
  auto res = Visibility{};
  for (; res.distance <= i; ++res.distance)
    if (grid[i - res.distance][j] >= grid[i][j])
      return res;
  --res.distance;
  res.visible = true;
  return res;
}

inline Visibility globalVisibility(const std::vector<std::vector<uint>> &grid,
                                   const std::size_t i, const std::size_t j) {
  const auto right = rightVisibility(grid, i, j);
  const auto left = leftVisibility(grid, i, j);
  const auto bottom = bottomVisibility(grid, i, j);
  const auto top = topVisibility(grid, i, j);
  return Visibility{
      right.visible or left.visible or bottom.visible or top.visible,
      right.distance * left.distance * bottom.distance * top.distance};
}

class Forest {
public:
  Forest(const QString &input) {
    const auto lines = common::splitLines(input);
    m_grid.reserve(lines.size());
    for (const auto &line : lines) {
      m_grid.emplace_back();
      m_grid.back().reserve(line.size());
      for (const auto &c : line)
        m_grid.back().emplace_back(QString(c).toUInt());
    }
  }

  uint nbVisible() const {
    auto sum = 0u;
    for (auto i = 0u; i < m_grid.size(); ++i)
      for (auto j = 0u; j < m_grid.size(); ++j)
        if (globalVisibility(m_grid, i, j).visible)
          ++sum;
    return sum;
  }

  uint maxScenicScore() const {
    auto max_score = 0u;
    for (auto i = 0u; i < m_grid.size(); ++i) {
      for (auto j = 0u; j < m_grid.size(); ++j) {
        const auto global_visibility = globalVisibility(m_grid, i, j);
        max_score = std::max(global_visibility.distance, max_score);
      }
    }
    return max_score;
  }

private:
  std::vector<std::vector<uint>> m_grid;
};

} // namespace puzzle_2022_08

void Solver_2022_08_1::solve(const QString &input) {
  const auto forest = puzzle_2022_08::Forest{input};
  emit finished(QString("%1").arg(forest.nbVisible()));
}

void Solver_2022_08_2::solve(const QString &input) {
  const auto forest = puzzle_2022_08::Forest{input};
  emit finished(QString("%1").arg(forest.maxScenicScore()));
}
