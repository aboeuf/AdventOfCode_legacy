#include <algorithm>
#include <solvers/2024/puzzle_2024_20.h>
#include <solvers/common.h>
#include <solvers/qpoint_hash.hpp>

namespace puzzle_2024_20 {

const auto cardinal_directions =
    QSet<QPoint>{QPoint{-1, 0}, QPoint{1, 0}, QPoint{0, -1}, QPoint{0, 1}};

uint manhattanDistance(const QPoint &A, const QPoint &B) {
  const auto diff = A - B;
  return static_cast<uint>(std::abs(diff.x()) + std::abs(diff.y()));
}

using CostMap = QHash<QPoint, uint>;

class Maze {
public:
  Maze(const QString &input) {
    const auto lines = common::splitLines(input, true);
    m_nb_rows = lines.size();
    if (m_nb_rows == 0) {
      common::throwInvalidArgumentError("puzzle_2024_20::Maze: empty input");
    }
    m_nb_columns = lines.front().size();
    auto row = 0;
    auto start_found = false;
    auto end_found = false;
    for (const auto &line : lines) {
      if (line.size() != m_nb_columns) {
        common::throwInvalidArgumentError(
            "puzzle_2024_20::Maze: incoherent number of columns");
      }
      auto column = 0;
      for (const auto &c : line) {
        if (c == 'S') {
          if (start_found) {
            common::throwInvalidArgumentError(
                "puzzle_2024_20::Maze: multiple start positions");
          }
          start_found = true;
          m_start = QPoint(row, column);
        } else if (c == 'E') {
          if (end_found) {
            common::throwInvalidArgumentError(
                "puzzle_2024_20::Maze: multiple end positions");
          }
          end_found = true;
          m_end = QPoint(row, column);
        } else if (c == '#') {
          m_walls.insert(QPoint(row, column));
        } else if (c != '.') {
          common::throwInvalidArgumentError(
              QString("puzzle_2024_20::Maze: unrecognized character '%1")
                  .arg(c));
        }
        ++column;
      }
      ++row;
    }
  }

  QString solve(const uint max_cheat_duration,
                const uint minimum_cost_gain) const {
    const auto cost_map = computeCostMap();
    auto nb_cheats_over_gain_limit = 0u;
    for (auto start_it = std::cbegin(cost_map); start_it != std::cend(cost_map);
         ++start_it) {
      for (auto end_it = std::next(start_it); end_it != std::cend(cost_map);
           ++end_it) {
        const auto cheat_cost = manhattanDistance(start_it.key(), end_it.key());
        if (cheat_cost <= max_cheat_duration) {
          auto start_cost = start_it.value();
          auto end_cost = end_it.value();
          if (start_cost > end_cost) {
            std::swap(start_cost, end_cost);
          }
          if (start_cost + cheat_cost < end_cost) {
            const auto cost_gain = end_cost - start_cost - cheat_cost;
            if (cost_gain >= minimum_cost_gain) {
              ++nb_cheats_over_gain_limit;
            }
          }
        }
      }
    }
    return QString("%1").arg(nb_cheats_over_gain_limit);
  }

private:
  bool isValidPosition(const QPoint &point) const {
    return point.x() >= 0 and point.x() < m_nb_rows and point.y() >= 0 and
           point.y() < m_nb_columns and not m_walls.contains(point);
  }

  QHash<QPoint, uint> computeCostMap() const {
    auto cost_map = CostMap();
    cost_map.insert(m_start, 0u);

    const auto get_accumulated_cost = [&cost_map](const QPoint &point) {
      const auto it = cost_map.constFind(point);
      if (it == cost_map.end()) {
        return std::numeric_limits<uint>::max();
      }
      return *it;
    };

    const auto get_cost = [this, &cost_map](const QPoint &point) {
      const auto it = cost_map.constFind(point);
      if (it == cost_map.end()) {
        return std::numeric_limits<uint>::max();
      }
      return *it + manhattanDistance(point, m_end);
    };

    const auto compare = [&get_cost](const QPoint &lhs, const QPoint &rhs) {
      return get_cost(rhs) < get_cost(lhs);
    };

    auto open_set = std::vector<QPoint>();
    auto unique = QSet<QPoint>();
    open_set.reserve(m_nb_rows * m_nb_columns);
    open_set.emplace_back(m_start);
    unique.insert(m_start);
    std::make_heap(std::begin(open_set), std::end(open_set), compare);

    while (not std::empty(open_set)) {
      const auto best = open_set.front();
      std::pop_heap(std::begin(open_set), std::end(open_set), compare);
      open_set.pop_back();
      unique.remove(best);
      const auto next_cost = get_accumulated_cost(best) + 1u;
      for (const auto &direction : cardinal_directions) {
        const auto neighbor = best + direction;
        if (isValidPosition(neighbor) and
            next_cost < get_accumulated_cost(neighbor)) {
          cost_map[neighbor] = next_cost;
          if (not unique.contains(neighbor)) {
            unique.insert(neighbor);
            open_set.push_back(neighbor);
          }
          std::push_heap(std::begin(open_set), std::end(open_set), compare);
        }
      }
    }
    return cost_map;
  }

  int m_nb_rows;
  int m_nb_columns;
  QSet<QPoint> m_walls;
  QPoint m_start;
  QPoint m_end;
};

} // namespace puzzle_2024_20

void Solver_2024_20_1::solve(const QString &input) {
  const auto maze = puzzle_2024_20::Maze(input);
  emit finished(maze.solve(2u, 100u));
}

void Solver_2024_20_2::solve(const QString &input) {
  const auto maze = puzzle_2024_20::Maze(input);
  emit finished(maze.solve(20u, 100u));
}
