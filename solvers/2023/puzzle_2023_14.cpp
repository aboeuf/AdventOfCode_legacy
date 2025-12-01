#include <QBitArray>
#include <solvers/common.h>
#include <solvers/2023/puzzle_2023_14.h>

namespace puzzle_2023_14 {

enum class Cell { Rounded, Cube, Empty };

enum class Direction { North, West, South, East };

enum class Orientation { Vertical, Horizontal };

inline Orientation getOrientation(Direction direction) {
  if (direction == Direction::North or direction == Direction::South)
    return Orientation::Vertical;
  if (direction == Direction::West or direction == Direction::East)
    return Orientation::Horizontal;
  common::throwInvalidArgumentError("unknown direction");
  return Orientation::Vertical;
}

struct IndexAndLoad {
  IndexAndLoad() = default;
  IndexAndLoad(uint index, uint load) : index{index}, load{load} {}
  uint index{0};
  uint load{0};
};

class ControlPanel {
public:
  ControlPanel(const QString &input) {
    auto lines = common::splitLines(input);
    m_lenght = lines.size();
    m_cells.reserve(m_lenght);
    if (m_lenght == 0)
      common::throwInvalidArgumentError("empty pannel");
    m_width = lines.front().size();
    for (const auto &line : lines) {
      if (line.size() != m_width)
        common::throwInvalidArgumentError("incoherent width");
      m_cells.emplace_back();
      m_cells.back().reserve(m_width);
      for (const auto &c : line) {
        if (c == 'O')
          m_cells.back().emplace_back(Cell::Rounded);
        else if (c == '#')
          m_cells.back().emplace_back(Cell::Cube);
        else if (c == '.')
          m_cells.back().emplace_back(Cell::Empty);
        else
          common::throwInvalidArgumentError("unknown cell type");
      }
    }
  }

  QBitArray toBitArray() const {
    auto array = QBitArray(m_lenght * m_width);
    auto bit_index = 0;
    for (auto i = 0; i < m_lenght; ++i) {
      for (auto j = 0; j < m_width; ++j) {
        array.setBit(bit_index++, m_cells[i][j] == Cell::Rounded);
      }
    }
    return array;
  }

  bool isFree(int i, int j) {
    return i >= 0 and i < m_lenght and j >= 0 and j < m_width and
           m_cells[i][j] == Cell::Empty;
  }

  int getShift(Direction direction, int i, int j) {
    auto shift = 0;
    if (direction == Direction::North)
      for (; isFree(i + shift - 1, j); --shift) {
      }
    else if (direction == Direction::West)
      for (; isFree(j, i + shift - 1); --shift) {
      }
    else if (direction == Direction::South)
      for (; isFree(i + shift + 1, j); ++shift) {
      }
    else if (direction == Direction::East)
      for (; isFree(j, i + shift + 1); ++shift) {
      }
    return shift;
  }

  void tiltLine(Direction direction, int j) {
    const auto orientation = getOrientation(direction);
    const auto max =
        (orientation == Orientation::Horizontal ? m_width : m_lenght);
    const auto max_othogonal =
        (orientation == Orientation::Horizontal ? m_lenght : m_width);
    if (j >= 0 and j < max_othogonal) {
      const auto start =
          (direction == Direction::North or direction == Direction::West)
              ? 1
              : max - 2;
      const auto go_on = [direction, max](int i) {
        if (direction == Direction::North or direction == Direction::West)
          return i < max;
        return i >= 0;
      };
      const auto increment = [direction, max](int &i) {
        if (direction == Direction::North or direction == Direction::West)
          ++i;
        else
          --i;
      };
      for (auto i = start; go_on(i); increment(i)) {
        const auto cell = orientation == Orientation::Horizontal
                              ? m_cells[j][i]
                              : m_cells[i][j];
        if (cell == Cell::Rounded) {
          const auto shift = getShift(direction, i, j);
          if (shift != 0) {
            if (orientation == Orientation::Horizontal) {
              m_cells[j][i + shift] = Cell::Rounded;
              m_cells[j][i] = Cell::Empty;
            } else {
              m_cells[i + shift][j] = Cell::Rounded;
              m_cells[i][j] = Cell::Empty;
            }
          }
        }
      }
    }
  }

  void tilt(Direction direction) {
    const auto orientation = getOrientation(direction);
    const auto max =
        (orientation == Orientation::Horizontal ? m_width : m_lenght);
    for (auto index = 0; index < max; ++index)
      tiltLine(direction, index);
  }

  uint getLoad() const {
    auto load = 0u;
    for (auto i = 0; i < m_lenght; ++i) {
      for (auto j = 0; j < m_width; ++j) {
        if (m_cells[i][j] == Cell::Rounded) {
          load += static_cast<uint>(m_width - i);
        }
      }
    }
    return load;
  }

  void tiltCycle() {
    tilt(Direction::North);
    tilt(Direction::West);
    tilt(Direction::South);
    tilt(Direction::East);
  }

  QString solveOne() {
    tilt(Direction::North);
    return QString("%1").arg(getLoad());
  }

  QString solveTwo() {
    const auto nb_cycles = 1000000000u;
    auto index_to_array = std::vector<QBitArray>{};
    index_to_array.reserve(nb_cycles);
    auto array_to_index_and_load = QHash<QBitArray, IndexAndLoad>{};
    auto loop_start = 0u;
    for (auto index = 0u;; ++index) {
      tiltCycle();
      const auto array = toBitArray();
      if (array_to_index_and_load.contains(array)) {
        loop_start = array_to_index_and_load[array].index;
        break;
      }
      array_to_index_and_load[array] = IndexAndLoad{index, getLoad()};
      index_to_array.emplace_back(array);
    }
    const auto loop_end = index_to_array.size();
    const auto loop_size = loop_end - loop_start;
    const auto nb_cycles_mod = (nb_cycles - loop_start) % loop_size;
    const auto index = nb_cycles_mod + loop_start - 1u;
    const auto load = array_to_index_and_load[index_to_array[index]].load;
    return QString("%1").arg(load);
  }

private:
  std::vector<std::vector<Cell>> m_cells;
  int m_lenght{0};
  int m_width{0};
};

} // namespace puzzle_2023_14

void Solver_2023_14_1::solve(const QString &input) {
  auto pannel = puzzle_2023_14::ControlPanel{input};
  emit finished(pannel.solveOne());
}

void Solver_2023_14_2::solve(const QString &input) {
  auto pannel = puzzle_2023_14::ControlPanel{input};
  emit finished(pannel.solveTwo());
}
