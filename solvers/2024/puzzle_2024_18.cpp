#include <solvers/common.h>
#include <functional>
#include <limits>
#include <solvers/qpoint_hash.hpp>
#include <solvers/2024/puzzle_2024_18.h>

namespace puzzle_2024_18 {

struct Tile {
  uint cost() const {
    if (accumulated_cost.has_value()) {
      return *accumulated_cost + heuristic_cost;
    }
    return std::numeric_limits<uint>::max();
  }
  uint heuristic_cost;
  std::optional<uint> accumulated_cost{std::nullopt};
  QList<QPoint> neighbors{};
  std::optional<QPoint> previous{std::nullopt};
};

class Grid {
public:
  Grid(const QPoint &goal_position, int grid_size)
      : m_grid(std::vector<std::vector<Tile>>(
            grid_size + 1, std::vector<Tile>(grid_size + 1))) {
    static const auto cardinal_directions =
        QSet<QPoint>{QPoint(-1, 0), QPoint(1, 0), QPoint(0, -1), QPoint(0, 1)};
    for (auto x = 0; x <= grid_size; ++x) {
      for (auto y = 0; y <= grid_size; ++y) {
        const auto position = QPoint(x, y);
        const auto diff = goal_position - position;
        m_grid[x][y].heuristic_cost =
            static_cast<uint>(std::abs(diff.x()) + std::abs(diff.y()));
        for (const auto &direction : cardinal_directions) {
          const auto neighbor = position + direction;
          if (neighbor.x() >= 0 and neighbor.x() <= grid_size and
              neighbor.y() >= 0 and neighbor.y() <= grid_size) {
            m_grid[x][y].neighbors << neighbor;
          }
        }
      }
    }
  }

  const Tile &get(const QPoint &position) const {
    return m_grid[position.x()][position.y()];
  }

  Tile &get(const QPoint &position) {
    return m_grid[position.x()][position.y()];
  }

private:
  std::vector<std::vector<Tile>> m_grid;
};

using Compare = std::function<bool(const QPoint &, const QPoint &)>;

class OpenSet {
public:
  OpenSet(const Compare &compare, int grid_size) : m_compare{compare} {
    m_heap.reserve((grid_size + 1) * (grid_size + 1));
    std::make_heap(std::begin(m_heap), std::end(m_heap), m_compare);
  }

  bool empty() const { return std::empty(m_heap); }

  QPoint pop() {
    const auto point = m_heap.front();
    m_unique.remove(point);
    std::pop_heap(std::begin(m_heap), std::end(m_heap), m_compare);
    m_heap.pop_back();
    return point;
  }

  void push(const QPoint &point) {
    if (not m_unique.contains(point)) {
      m_unique.insert(point);
      m_heap.push_back(point);
    }
    std::push_heap(std::begin(m_heap), std::end(m_heap), m_compare);
  }

private:
  Compare m_compare;
  std::vector<QPoint> m_heap;
  QSet<QPoint> m_unique;
};

class MemorySpace {
public:
  MemorySpace(const QString &input, int size) : m_size(size) {
    const auto lines = common::splitLines(input, true);
    auto time = 1u;
    m_falling_bytes_by_times.reserve(lines.size());
    for (const auto &line : lines) {
      try {
        const auto coordinates = common::toVecInt(line);
        if (coordinates.size() != 2) {
          common::throwInvalidArgumentError(
              QString("wrong size (%1 instead of 2)").arg(coordinates.size()));
        }
        const auto coords = QPoint(coordinates.front(), coordinates.back());
        m_falling_bytes_by_coordinates[coords] = time;
        m_falling_bytes_by_times.emplace_back(coords);
      } catch (const std::exception &e) {
        common::throwInvalidArgumentError(
            QString(
                "puzzle_2024_18::SpaceTimePoint: cannot parse string \"%1\" "
                "because of error: %2")
                .arg(input)
                .arg(e.what()));
      }
      ++time;
    }
  }

  QString solveOne(uint time) const {
    const auto start_position = QPoint(0, 0);
    const auto goal_position = QPoint(m_size, m_size);
    const auto result = solveAtTime(start_position, goal_position, time);
    if (result.first) {
      return QString("%1").arg(
          *result.second.get(goal_position).accumulated_cost);
    }
    return "No solution";
  }

  QString solveTwo() const {
    const auto start_position = QPoint(0, 0);
    const auto goal_position = QPoint(m_size, m_size);
    for (auto time = 1u; time <= std::size(m_falling_bytes_by_times); ++time) {
      const auto result = solveAtTime(start_position, goal_position, time);
      const auto &blocker = m_falling_bytes_by_times[time - 1];
      if (not result.first) {
        return QString("%1,%2").arg(blocker.x()).arg(blocker.y());
      }
    }
    return "No blockers";
  }

private:
  bool isFreeAtTime(const QPoint &position, uint time) const {
    const auto it = m_falling_bytes_by_coordinates.constFind(position);
    if (it == m_falling_bytes_by_coordinates.cend()) {
      return true;
    }
    return time < it.value();
  }

  std::pair<bool, Grid> solveAtTime(const QPoint &start, const QPoint &goal,
                                    uint time) const {
    auto result = std::make_pair(false, Grid(goal, m_size));
    auto &grid = result.second;
    const auto compare = [&grid](const QPoint &lhs, const QPoint &rhs) {
      return grid.get(rhs).cost() < grid.get(lhs).cost();
    };
    auto open_set = OpenSet(compare, m_size);
    grid.get(start).accumulated_cost = 0u;
    open_set.push(start);

    while (not open_set.empty()) {
      const auto position = open_set.pop();
      if (position == goal) {
        result.first = true;
        return result;
      }
      const auto &tile = grid.get(position);
      const auto next_accumulated_cost = *tile.accumulated_cost + 1u;
      for (const auto &neighbor_postion : tile.neighbors) {
        if (isFreeAtTime(neighbor_postion, time)) {
          auto &neighbor_tile = grid.get(neighbor_postion);
          if (not neighbor_tile.accumulated_cost.has_value() or
              next_accumulated_cost < *neighbor_tile.accumulated_cost) {
            neighbor_tile.accumulated_cost = next_accumulated_cost;
            neighbor_tile.previous = position;
            open_set.push(neighbor_postion);
          }
        }
      }
    }
    return result;
  }

  int m_size;
  std::vector<QPoint> m_falling_bytes_by_times;
  QHash<QPoint, uint> m_falling_bytes_by_coordinates;
};

} // namespace puzzle_2024_18

void Solver_2024_18_1::solve(const QString &input) {
  const auto memory_space = puzzle_2024_18::MemorySpace(input, 70);
  emit finished(memory_space.solveOne(1024u));
}

void Solver_2024_18_2::solve(const QString &input) {
  const auto memory_space = puzzle_2024_18::MemorySpace(input, 70);
  emit finished(memory_space.solveTwo());
}
