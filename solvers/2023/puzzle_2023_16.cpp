#include <QQueue>
#include <solvers/common.h>
#include <set>
#include <solvers/2023/puzzle_2023_16.h>

namespace puzzle_2023_16 {

struct Coordinates {
  Coordinates() = default;
  Coordinates(int x, int y) : x{x}, y{y} {}
  int x{-1};
  int y{-1};
};

bool operator<(const Coordinates &lhs, const Coordinates &rhs) {
  if (lhs.x == rhs.x)
    return lhs.y < rhs.y;
  return lhs.x < rhs.x;
}

enum class Direction { north, west, south, east };

struct TileState {
  TileState() = default;

  bool turnOutputOn(Direction output) {
    if (output == Direction::north) {
      if (not north) {
        north = true;
        return true;
      }
      return false;
    }

    if (output == Direction::west) {
      if (not west) {
        west = true;
        return true;
      }
      return false;
    }

    if (output == Direction::south) {
      if (not south) {
        south = true;
        return true;
      }
      return false;
    }

    if (output == Direction::east) {
      if (not east) {
        east = true;
        return true;
      }
      return false;
    }

    common::throwInvalidArgumentError("turnOutputOn: unknown direction");
    return false;
  }

  TileState inputBeam(const QChar &type, Direction direction) {
    auto changes_ouputs = TileState{};
    if (type == '.') {
      if (direction == Direction::north) {
        changes_ouputs.south = turnOutputOn(Direction::south);
      } else if (direction == Direction::west) {
        changes_ouputs.east = turnOutputOn(Direction::east);
      } else if (direction == Direction::south) {
        changes_ouputs.north = turnOutputOn(Direction::north);
      } else if (direction == Direction::east) {
        changes_ouputs.west = turnOutputOn(Direction::west);
      } else {
        common::throwInvalidArgumentError("unknown direction");
      }
    } else if (type == '-') {
      if (direction == Direction::north or direction == Direction::south) {
        changes_ouputs.west = turnOutputOn(Direction::west);
        changes_ouputs.east = turnOutputOn(Direction::east);
      } else if (direction == Direction::west) {
        changes_ouputs.east = turnOutputOn(Direction::east);
      } else if (direction == Direction::east) {
        changes_ouputs.west = turnOutputOn(Direction::west);
      } else {
        common::throwInvalidArgumentError("unknown direction");
      }
    } else if (type == '|') {
      if (direction == Direction::west or direction == Direction::east) {
        changes_ouputs.north = turnOutputOn(Direction::north);
        changes_ouputs.south = turnOutputOn(Direction::south);
      } else if (direction == Direction::north) {
        changes_ouputs.south = turnOutputOn(Direction::south);
      } else if (direction == Direction::south) {
        changes_ouputs.north = turnOutputOn(Direction::north);
      } else {
        common::throwInvalidArgumentError("unknown direction");
      }
    } else if (type == '/') {
      if (direction == Direction::north) {
        changes_ouputs.west = turnOutputOn(Direction::west);
      } else if (direction == Direction::west) {
        changes_ouputs.north = turnOutputOn(Direction::north);
      } else if (direction == Direction::south) {
        changes_ouputs.east = turnOutputOn(Direction::east);
      } else if (direction == Direction::east) {
        changes_ouputs.south = turnOutputOn(Direction::south);
      } else {
        common::throwInvalidArgumentError("unknown direction");
      }
    } else if (type == '\\') {
      if (direction == Direction::north) {
        changes_ouputs.east = turnOutputOn(Direction::east);
      } else if (direction == Direction::west) {
        changes_ouputs.south = turnOutputOn(Direction::south);
      } else if (direction == Direction::south) {
        changes_ouputs.west = turnOutputOn(Direction::west);
      } else if (direction == Direction::east) {
        changes_ouputs.north = turnOutputOn(Direction::north);
      } else {
        common::throwInvalidArgumentError("unknown direction");
      }
    } else {
      common::throwInvalidArgumentError("unknown cell type");
    }
    return changes_ouputs;
  }

  bool north{false};
  bool west{false};
  bool south{false};
  bool east{false};
};

struct SearchState {
  SearchState() = default;
  SearchState(int x, int y) : coordinates{x, y} {}
  SearchState(int x, int y, Direction direction) : coordinates{x, y} {
    changed_state.turnOutputOn(direction);
  }
  Coordinates coordinates{};
  TileState changed_state{};
};

class Grid {
public:
  Grid(const QString &input) {
    const auto lines = common::splitLines(input);
    m_length = lines.size();
    if (m_length == 0)
      common::throwInvalidArgumentError("empty grid");
    m_tiles.reserve(m_length);
    m_width = lines.front().size();
    for (const auto &line : lines) {
      if (line.size() != m_width)
        common::throwInvalidArgumentError("incoherent line size");
      m_tiles.emplace_back();
      m_tiles.back().reserve(m_width);
      for (const auto &c : line)
        m_tiles.back().emplace_back(c);
    }
  }

  uint inputBeamAt(int x_init, int y_init, Direction direction_init) const {
    const auto init_state = SearchState{x_init, y_init, direction_init};
    auto map = QMap<Coordinates, TileState>{};
    map[init_state.coordinates] = init_state.changed_state;

    auto queue = QQueue<SearchState>{};
    queue.enqueue(init_state);

    const auto enqueue = [&queue, &map, this](int x, int y,
                                              Direction direction) {
      if (x < 0 or x >= m_length or y < 0 or y >= m_width)
        return;
      auto next_state = SearchState{x, y};
      if (not map.contains(next_state.coordinates))
        map[next_state.coordinates] = TileState{};
      next_state.changed_state =
          map[next_state.coordinates].inputBeam(m_tiles[x][y], direction);
      queue.enqueue(next_state);
    };

    while (not queue.empty()) {
      const auto current = queue.dequeue();
      const auto x = current.coordinates.x;
      const auto y = current.coordinates.y;
      if (current.changed_state.north)
        enqueue(x - 1, y, Direction::south);
      if (current.changed_state.west)
        enqueue(x, y - 1, Direction::east);
      if (current.changed_state.south)
        enqueue(x + 1, y, Direction::north);
      if (current.changed_state.east)
        enqueue(x, y + 1, Direction::west);
    }

    return static_cast<uint>(map.size());
  }

  QString solveOne() {
    return QString("%1").arg(inputBeamAt(0, 0, Direction::east));
  }

  QString solveTwo() {
    auto maximum = 0u;
    for (auto x = 0; x < m_length; ++x) {
      maximum = std::max(maximum, inputBeamAt(x, 0, Direction::east));
      maximum = std::max(maximum, inputBeamAt(x, m_width - 1, Direction::west));
    }
    for (auto y = 0; y < m_length; ++y) {
      maximum = std::max(maximum, inputBeamAt(0, y, Direction::south));
      maximum =
          std::max(maximum, inputBeamAt(m_length - 1, y, Direction::north));
    }
    return QString("%1").arg(maximum);
  }

private:
  std::vector<std::vector<QChar>> m_tiles;
  int m_length;
  int m_width;
};

} // namespace puzzle_2023_16

void Solver_2023_16_1::solve(const QString &input) {
  auto grid = puzzle_2023_16::Grid{input};
  emit finished(grid.solveOne());
}

void Solver_2023_16_2::solve(const QString &input) {
  auto grid = puzzle_2023_16::Grid{input};
  emit finished(grid.solveTwo());
}
