#include <algorithm>
#include <solvers/common.h>
#include <functional>
#include <iostream>
#include <limits>
#include <solvers/qpoint_hash.hpp>
#include <solvers/2024/puzzle_2024_16.h>

namespace puzzle_2024_16 {

constexpr auto translation_cost = 1u;
constexpr auto rotation_cost = 1000u;

enum class Orientation { north, west, south, east };

const auto orientations =
    std::array<Orientation, 4u>{Orientation::north, Orientation::west,
                                Orientation::south, Orientation::east};

inline QChar orientationToChar(Orientation orientation) {
  switch (orientation) {
  case Orientation::north:
    return '^';
  case Orientation::west:
    return '<';
  case Orientation::south:
    return 'v';
  case Orientation::east:
    return '>';
  }
  common::throwInvalidArgumentError(
      "puzzle_2024_16::orientationToChar: unknown orientation");
  return '#';
}

inline QPoint orientationToPoint(Orientation orientation) {
  switch (orientation) {
  case Orientation::north:
    return QPoint(-1, 0);
  case Orientation::west:
    return QPoint(0, -1);
  case Orientation::south:
    return QPoint(1, 0);
  case Orientation::east:
    return QPoint(0, 1);
  }
  common::throwInvalidArgumentError(
      "puzzle_2024_16::orientationToChar: unknown orientation");
  return QPoint();
}

inline Orientation pointToOrientation(const QPoint &point) {
  if (point.x() == 0) {
    if (point.y() < 0) {
      return Orientation::west;
    }
    if (point.y() > 0) {
      return Orientation::east;
    }
  }
  if (point.y() == 0) {
    if (point.x() < 0) {
      return Orientation::north;
    }
    if (point.x() > 0) {
      return Orientation::south;
    }
  }
  common::throwInvalidArgumentError(
      QString(
          "puzzle_2024_16::pointToOrientation: invalid orientation (%1, %2)")
          .arg(point.x())
          .arg(point.x()));
  return Orientation::east;
}

inline uint manhattanDistance(const QPoint &A, const QPoint &B) {
  const auto diff = A - B;
  return static_cast<uint>(std::abs(diff.x()) + std::abs(diff.y()));
}

struct Node {
  Node(const QPoint &position, Orientation orientation)
      : position{position}, orientation{orientation} {}

  Node(const QPoint &position, const QPoint &orientation)
      : Node{position, pointToOrientation(orientation)} {}

  bool operator==(const Node &other) const {
    return position == other.position and orientation == other.orientation;
  }

  QPoint position;
  Orientation orientation;
};

inline uint qHash(const Node &node) {
  return qHash(QString("%1%2%3")
                   .arg(node.position.x())
                   .arg(orientationToChar(node.orientation))
                   .arg(node.position.y()));
}

struct NodeData {
  uint cost() const {
    if (accumulated_cost.has_value()) {
      return *accumulated_cost + heuristic_cost;
    }
    return std::numeric_limits<uint>::max();
  }

  uint heuristic_cost{0u};
  std::optional<uint> accumulated_cost{std::nullopt};
  QHash<Node, uint> outgoing_edges{};
  QSet<Node> best_parents{};
  bool in_open_set{false};
};

class Maze {
public:
  Maze(const QString &input) {
    const auto lines = common::splitLines(input, true);
    m_nb_rows = lines.size();
    if (m_nb_rows == 0) {
      common::throwInvalidArgumentError("puzzle_2024_16::Maze: empty input");
    }
    m_nb_columns = lines.front().size();
    auto row = 0;
    auto start_found = false;
    auto end_found = false;
    for (const auto &line : lines) {
      if (line.size() != m_nb_columns) {
        common::throwInvalidArgumentError(
            "puzzle_2024_16::Maze: incoherent number of columns");
      }
      auto column = 0;
      for (const auto &c : line) {
        if (c == 'S') {
          if (start_found) {
            common::throwInvalidArgumentError(
                "puzzle_2024_16::Maze: multiple start positions");
          }
          start_found = true;
          m_start = QPoint(row, column);
        } else if (c == 'E') {
          if (end_found) {
            common::throwInvalidArgumentError(
                "puzzle_2024_16::Maze: multiple end positions");
          }
          end_found = true;
          m_end = QPoint(row, column);
        } else if (c == '#') {
          m_walls.insert(QPoint(row, column));
        } else if (c != '.') {
          common::throwInvalidArgumentError(
              QString("puzzle_2024_16::Maze: unrecognized character '%1")
                  .arg(c));
        }
        ++column;
      }
      ++row;
    }
  }

  QString solve(bool v2) const {
    // Create nodes
    auto nodes = QHash<Node, NodeData>();
    for (auto i = 0; i < m_nb_rows; ++i) {
      for (auto j = 0; j < m_nb_columns; ++j) {
        const auto position = QPoint(i, j);
        if (not m_walls.contains(position)) {
          for (const auto &orientation : orientations) {
            const auto node = Node(position, orientation);
            nodes[node].heuristic_cost = manhattanDistance(position, m_end);
          }
        }
      }
    }

    // Link nodes
    for (auto it = std::begin(nodes); it != std::end(nodes); ++it) {
      auto &node = it.key();
      auto &data = it.value();
      auto next_orientation = orientationToPoint(node.orientation);
      const auto next_position = node.position + next_orientation;
      if (not m_walls.contains(next_position)) {
        data.outgoing_edges[Node(next_position, node.orientation)] =
            translation_cost;
      }
      next_orientation.ry() *= -1;
      std::swap(next_orientation.rx(), next_orientation.ry());
      data.outgoing_edges[Node(node.position, next_orientation)] =
          rotation_cost;
      next_orientation *= -1;
      data.outgoing_edges[Node(node.position, next_orientation)] =
          rotation_cost;
    }

    // Cost compare for max heap
    const auto compare = [&nodes](const Node &lhs, const Node &rhs) {
      return nodes[rhs].cost() < nodes[lhs].cost();
    };

    // Initialize open set
    auto open_set = std::vector<Node>();
    open_set.reserve(nodes.size());
    const auto root_node = Node(m_start, Orientation::east);
    auto &root_data = nodes[root_node];
    root_data.accumulated_cost = 0u;
    root_data.in_open_set = true;
    open_set.emplace_back(root_node);
    std::make_heap(std::begin(open_set), std::end(open_set), compare);

    // Run A*
    while (not std::empty(open_set)) {
      const auto node = open_set.front();
      auto &data = nodes[node];
      data.in_open_set = false;
      std::pop_heap(std::begin(open_set), std::end(open_set), compare);
      open_set.pop_back();
      if (not v2 and node.position == m_end) {
        return QString("%1").arg(data.cost());
      }
      for (auto it = std::begin(data.outgoing_edges);
           it != std::end(data.outgoing_edges); ++it) {
        const auto new_accumulated_cost = *data.accumulated_cost + it.value();
        auto &child_data = nodes[it.key()];
        if (not child_data.accumulated_cost.has_value() or
            new_accumulated_cost <= *child_data.accumulated_cost) {
          if (child_data.accumulated_cost.has_value() and
              new_accumulated_cost < *child_data.accumulated_cost) {
            child_data.best_parents.clear();
          }
          child_data.best_parents.insert(node);
          child_data.accumulated_cost = new_accumulated_cost;
          if (not child_data.in_open_set) {
            open_set.emplace_back(it.key());
          }
          child_data.in_open_set = true;
        }
      }
      std::push_heap(std::begin(open_set), std::end(open_set), compare);
    }

    // V1 failure
    if (not v2) {
      return "No solution";
    }

    // Compute best paths tiles;
    auto tiles_in_best_paths = QSet<QPoint>();
    std::function<void(const Node &)> add_tiles;
    add_tiles = [&nodes, &tiles_in_best_paths, &add_tiles](const Node &node) {
      tiles_in_best_paths.insert(node.position);
      for (const auto &parent : nodes[node].best_parents) {
        add_tiles(parent);
      }
    };
    for (auto orientation : orientations) {
      add_tiles(Node(m_end, orientation));
    }

    auto maze = toString();
    for (const auto &tile : tiles_in_best_paths) {
      maze[tile.x()][tile.y()] = 'O';
    }
    for (const auto &row : maze) {
      std::cout << row.toStdString() << std::endl;
    }

    return QString("%1").arg(tiles_in_best_paths.size());
  }

private:
  QStringList toString() const {
    auto res = QStringList();
    for (auto i = 0; i < m_nb_rows; ++i) {
      res << QString();
      for (auto j = 0; j < m_nb_columns; ++j) {
        res.back().push_back(m_walls.contains(QPoint(i, j)) ? '#' : '.');
      }
    }
    return res;
  }

  int m_nb_rows;
  int m_nb_columns;
  QSet<QPoint> m_walls;
  QPoint m_start;
  QPoint m_end;
};

} // namespace puzzle_2024_16

void Solver_2024_16_1::solve(const QString &input) {
  const auto maze = puzzle_2024_16::Maze(input);
  emit finished(maze.solve(false));
}

void Solver_2024_16_2::solve(const QString &input) {
  const auto maze = puzzle_2024_16::Maze(input);
  emit finished(maze.solve(true));
}
