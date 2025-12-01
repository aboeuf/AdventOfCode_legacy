#include <solvers/common.h>
#include <solvers/2024/puzzle_2024_06.h>
#include <unordered_set>

#include <iostream>

namespace puzzle_2024_06 {

struct Coordinates {
  Coordinates() = default;

  Coordinates(int row, int column) : row{row}, column{column} {}

  void operator+=(const Coordinates &other) {
    row += other.row;
    column += other.column;
  }

  void operator-=(const Coordinates &other) {
    row -= other.row;
    column -= other.column;
  }

  bool operator==(const Coordinates &other) const {
    return row == other.row and column == other.column;
  }

  bool operator!=(const Coordinates &other) const {
    return row != other.row or column != other.column;
  }

  void rotateClockwise() {
    std::swap(row, column);
    column *= -1;
  }

  int row{0};
  int column{0};
};

inline std::size_t normalize(const int value) {
  return value < 0 ? 2u * static_cast<std::size_t>(-value) + 1u
                   : 2u * static_cast<std::size_t>(value);
}

} // namespace puzzle_2024_06

template <> struct std::hash<puzzle_2024_06::Coordinates> {
  std::size_t operator()(const puzzle_2024_06::Coordinates &coordinates) const {
    const auto normalized_row = puzzle_2024_06::normalize(coordinates.row);
    const auto normalized_column =
        puzzle_2024_06::normalize(coordinates.column);
    const auto sum = normalized_row + normalized_column;
    return sum * (sum + 1u) / 2 + normalized_column;
  }
};

namespace puzzle_2024_06 {

using Record = std::unordered_map<Coordinates, std::unordered_set<Coordinates>>;

struct SimulationResult {
  bool is_looping{false};
  Record record{};
};

class Map {
public:
  Map(const QString &input) {
    const auto lines = common::splitLines(input);
    m_nb_rows = lines.size();
    if (m_nb_rows == 0) {
      throw std::invalid_argument("puzzle_2024_06::Map: empty map");
    }
    m_nb_columns = lines.front().size();
    auto gard_position_found = false;
    auto row = 0;
    for (const auto &line : lines) {
      if (line.size() != m_nb_columns) {
        throw std::invalid_argument(
            QString("puzzle_2024_06::Map: incoherent number of columns at line "
                    "%1 (expected %2 but got %3")
                .arg(row + 1u)
                .arg(m_nb_columns)
                .arg(line.size())
                .toStdString());
      }
      auto column = 0;
      for (const auto &c : line) {
        if (c == '#') {
          m_obstacles.insert(Coordinates{row, column});
        } else if (c == '^' or c == '>' or c == 'v' or c == '<') {
          if (gard_position_found) {
            throw std::invalid_argument(
                QString("puzzle_2024_06::Map: multiple gard position detected "
                        "(line %1, column %2). Previous position was detected "
                        "at (line %3, column %4)"
                        "column %3")
                    .arg(m_initial_gard_position.row + 1u)
                    .arg(m_initial_gard_position.column + 1u)
                    .arg(row + 1u)
                    .arg(column + 1u)
                    .toStdString());
          }
          gard_position_found = true;
          m_initial_gard_position.row = row;
          m_initial_gard_position.column = column;
          if (c == '^') {
            m_initial_gard_orientation.row = -1;
          } else if (c == '>') {
            m_initial_gard_orientation.column = 1;
          } else if (c == 'v') {
            m_initial_gard_orientation.row = 1;
          } else {
            m_initial_gard_orientation.column = -1;
          }
        } else if (c != '.') {
          throw std::invalid_argument(
              QString("puzzle_2024_06::Map: invalid character '%1' at (line "
                      "%2, column %3")
                  .arg(c)
                  .arg(row + 1u)
                  .arg(column + 1u)
                  .toStdString());
        }
        ++column;
      }
      ++row;
    }
    if (not gard_position_found) {
      throw std::invalid_argument(
          "puzzle_2024_06::Map: cannot find gard position");
    }
  }

  SimulationResult runSimulation(
      const std::optional<Coordinates> &extra_obstacle = std::nullopt) const {
    auto result = SimulationResult{};
    auto gard_position = m_initial_gard_position;
    auto gard_orientation = m_initial_gard_orientation;
    result.record[gard_position].insert(gard_orientation);
    for (;;) {
      gard_position += gard_orientation;
      while (
          m_obstacles.find(gard_position) != std::cend(m_obstacles) or
          (extra_obstacle.has_value() and *extra_obstacle == gard_position)) {
        gard_position -= gard_orientation;
        gard_orientation.rotateClockwise();
        gard_position += gard_orientation;
      }
      if (gard_position.row < 0 or gard_position.row >= m_nb_rows or
          gard_position.column < 0 or gard_position.column >= m_nb_columns) {
        return result;
      }
      auto &orientations = result.record[gard_position];
      if (not orientations.insert(gard_orientation).second) {
        result.is_looping = true;
        return result;
      }
    }
  }

  QString solveOne() const {
    const auto result = runSimulation();
    return QString("%1").arg(std::size(result.record));
  }

  QString solveTwo() const {
    auto visited_positions = runSimulation().record;
    visited_positions.erase(m_initial_gard_position);
    auto nb_loops = 0u;
    for (const auto &[extra_obstacle, _] : visited_positions) {
      const auto result = runSimulation(extra_obstacle);
      if (result.is_looping) {
        ++nb_loops;
      }
    }
    return QString("%1").arg(nb_loops);
  }

private:
  std::unordered_set<Coordinates> m_obstacles;
  int m_nb_rows;
  int m_nb_columns;
  Coordinates m_initial_gard_position;
  Coordinates m_initial_gard_orientation;
};

} // namespace puzzle_2024_06

void Solver_2024_06_1::solve(const QString &input) {
  auto map = puzzle_2024_06::Map{input};
  emit finished(map.solveOne());
}

void Solver_2024_06_2::solve(const QString &input) {
  auto map = puzzle_2024_06::Map{input};
  emit finished(map.solveTwo());
}
