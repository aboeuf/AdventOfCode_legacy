#include <solvers/common.h>
#include <solvers/qpoint_hash.hpp>
#include <solvers/2024/puzzle_2024_15.h>

namespace puzzle_2024_15 {

const auto directions = QHash<QChar, QPoint>{{'^', QPoint(-1, 0)},
                                             {'>', QPoint(0, 1)},
                                             {'v', QPoint(1, 0)},
                                             {'<', QPoint(0, -1)}};

const QPoint &getDirection(const QChar &c) {
  const auto it = directions.constFind(c);
  if (it == directions.cend()) {
    common::throwInvalidArgumentError(
        QString("puzzle_2024_15::getDirection: unrecognized character '%1'")
            .arg(c));
  }
  return it.value();
}

enum class TileType { out_of_range, empty, box, wall, robot };

struct Tile {
  Tile(TileType type, QPoint position, QChar c)
      : type{type}, position{position}, c{c} {}
  TileType type;
  QPoint position;
  QChar c;
};

inline bool operator==(const Tile &lhs, const Tile &rhs) {
  return lhs.position == rhs.position;
}

inline uint qHash(const Tile &tile) { return qHash(tile.position); }

class WareHouse {
public:
  WareHouse(const QString &input, bool big) : m_big{big} {
    const auto lines = common::splitLines(input);
    if (lines.empty()) {
      common::throwInvalidArgumentError(
          "puzzle_2024_15::WareHouse: empty input");
    }
    const auto N = lines.front().size();
    m_nb_columns = (m_big ? 2 : 1) * N;
    for (m_nb_rows = 0; not lines[m_nb_rows].isEmpty(); ++m_nb_rows) {
      const auto nb_columns = (m_big ? 2 : 1) * lines[m_nb_rows].size();
      if (nb_columns != m_nb_columns) {
        common::throwInvalidArgumentError(
            "puzzle_2024_15::WareHouse: incoherent number of columns");
      }
    }
    if (m_nb_rows == 0) {
      common::throwInvalidArgumentError(
          "puzzle_2024_15::WareHouse: empty input");
    }
    m_tiles = std::vector<std::vector<QChar>>(
        static_cast<uint>(m_nb_rows),
        std::vector<QChar>(static_cast<uint>(m_nb_columns), '.'));
    auto robot_found = false;
    for (auto row = 0; row < m_nb_rows; ++row) {
      for (auto column = 0; column < N; ++column) {
        const auto &c = lines[row][column];
        const auto coordinates = QPoint(row, (m_big ? 2 : 1) * column);
        if (c == '@') {
          if (robot_found) {
            common::throwInvalidArgumentError(
                "puzzle_2024_15::WareHouse: multiple robots");
          }
          m_robot = coordinates;
          robot_found = true;
          m_tiles[coordinates.x()][coordinates.y()] = '@';
        } else if (c == 'O') {
          if (m_big) {
            m_tiles[coordinates.x()][coordinates.y()] = '[';
            m_tiles[coordinates.x()][coordinates.y() + 1] = ']';
          } else {
            m_tiles[coordinates.x()][coordinates.y()] = 'O';
          }
        } else if (c == '#') {
          m_tiles[coordinates.x()][coordinates.y()] = '#';
          if (m_big) {
            m_tiles[coordinates.x()][coordinates.y() + 1] = '#';
          }
        } else if (c != '.') {
          common::throwInvalidArgumentError(
              QString("puzzle_2024_15::WareHouse: unrecognized character '%1'")
                  .arg(c));
        }
      }
    }
    if (not robot_found) {
      common::throwInvalidArgumentError(
          "puzzle_2024_15::WareHouse: cannot find robot position");
    }
    for (auto line_index = m_nb_rows + 1; line_index < lines.size();
         ++line_index) {
      m_robot_moves += lines[line_index];
    }
  }

  QString toString() const {
    auto res = QString();
    for (auto row = 0; row < m_nb_rows; ++row) {
      for (auto column = 0; column < m_nb_columns; ++column) {
        res.push_back(m_tiles[row][column]);
      }
      res.push_back('\n');
    }
    return res;
  }

  QString solve() {
    for (const auto &c : m_robot_moves) {
      executeMove(c);
    }
    auto sum = 0;
    for (auto row = 0; row < m_nb_rows; ++row) {
      for (auto column = 0; column < m_nb_columns; ++column) {
        if ((m_big and m_tiles[row][column] == '[') or
            (not m_big and m_tiles[row][column] == 'O')) {
          sum += 100 * row + column;
        }
      }
    }
    return QString("%1").arg(sum);
  }

private:
  Tile getTile(const QPoint &position) const {
    if (position.x() < 0 or position.x() >= m_nb_rows or position.y() < 0 or
        position.y() >= m_nb_columns) {
      return Tile(TileType::out_of_range, position, '$');
    }
    const auto &c = m_tiles[position.x()][position.y()];
    if (c == '.') {
      return Tile(TileType::empty, position, c);
    }
    if (c == '@') {
      return Tile(TileType::robot, position, c);
    }
    if (c == '#') {
      return Tile(TileType::wall, position, c);
    }
    if ((m_big and (c == '[' or c == ']')) or (not m_big and c == 'O')) {
      return Tile(TileType::box, position, c);
    }
    common::throwRunTimeError(QString("puzzle_2024_15::WareHouse::getType: "
                                      "unrecognized character '%1'")
                                  .arg(c));
    return Tile(TileType::out_of_range, position, c);
  }

  void executeMove(const QChar &c) {
    const auto direction = getDirection(c);
    auto boxes = QList<QSet<Tile>>();

    const auto add_tile = [this, &direction](QSet<Tile> &layer,
                                             const Tile &tile) {
      layer.insert(tile);
      if (m_big and direction.x() != 0) {
        if (tile.c == '[') {
          layer.insert(getTile(tile.position + QPoint(0, 1)));
        } else if (tile.c == ']') {
          layer.insert(getTile(tile.position - QPoint(0, 1)));
        } else {
          common::throwRunTimeError(
              QString("puzzle_2024_15::WareHouse::executeMove::add_tile "
                      "executeMove unrecognized character '%1'")
                  .arg(tile.c));
        }
      }
    };

    for (;;) {
      auto next_layer = QSet<Tile>();
      if (boxes.isEmpty()) {
        const auto next_tile = getTile(m_robot + direction);
        if (next_tile.type == TileType::box) {
          add_tile(next_layer, next_tile);
        }
      } else {
        for (const auto &box : boxes.back()) {
          const auto next_tile = getTile(box.position + direction);
          if (next_tile.type == TileType::box) {
            add_tile(next_layer, next_tile);
          }
        }
      }
      if (next_layer.empty()) {
        break;
      }
      boxes << next_layer;
    }

    const auto can_move = [this, &boxes, &direction]() {
      if (boxes.empty()) {
        return getTile(m_robot + direction).type != TileType::wall;
      }
      for (const auto &layer : boxes) {
        for (const auto &box : layer) {
          if (getTile(box.position + direction).type == TileType::wall) {
            return false;
          }
        }
      }
      return true;
    }();

    if (can_move) {
      for (const auto &layer : boxes) {
        for (const auto &box : layer) {
          m_tiles[box.position.x()][box.position.y()] = '.';
        }
      }
      for (const auto &layer : boxes) {
        for (const auto &box : layer) {
          m_tiles[box.position.x() + direction.x()]
                 [box.position.y() + direction.y()] = box.c;
        }
      }
      m_tiles[m_robot.x()][m_robot.y()] = '.';
      m_robot += direction;
      m_tiles[m_robot.x()][m_robot.y()] = '@';
    }
  }

  bool m_big;
  int m_nb_rows;
  int m_nb_columns;
  std::vector<std::vector<QChar>> m_tiles;
  QPoint m_robot;
  QString m_robot_moves;
};

} // namespace puzzle_2024_15

void Solver_2024_15_1::solve(const QString &input) {
  auto warehouse = puzzle_2024_15::WareHouse(input, false);
  const auto result = warehouse.solve();
  emit output(warehouse.toString());
  emit finished(result);
}

void Solver_2024_15_2::solve(const QString &input) {
  auto warehouse = puzzle_2024_15::WareHouse(input, true);
  const auto result = warehouse.solve();
  emit output(warehouse.toString());
  emit finished(result);
}
