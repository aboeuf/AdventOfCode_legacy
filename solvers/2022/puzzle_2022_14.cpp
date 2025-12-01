#include <solvers/common.h>
#include <map>
#include <solvers/2022/puzzle_2022_14.h>

namespace puzzle_2022_14 {

using Int = long long int;
using UInt = long long unsigned int;

enum class Type { ROCK, SAND };

struct Position {
  Position() = default;
  Position(Int x, Int y) : x{x}, y{y} {}

  bool operator==(const Position &other) const {
    return x == other.x and y == other.y;
  }

  bool operator<(const Position &other) const {
    if (x == other.x)
      return y < other.y;
    return x < other.x;
  }

  Int x{0};
  Int y{0};
};

const auto sand_source = Position{500, 0};

struct Range {
  void update(const Int &value) {
    min = std::min(min, value);
    max = std::max(max, value);
  }

  Int min{std::numeric_limits<Int>::max()};
  Int max{std::numeric_limits<Int>::min()};
};

struct Range2D {
  void update(const Position &position) {
    x.update(position.x);
    y.update(position.y);
  }

  Range x{};
  Range y{};
};

class Cave {
public:
  Cave(const QString &input) {
    m_range.update(sand_source);
    auto lines = common::splitLines(input);
    for (auto &line : lines) {
      line.replace(" -> ", "@");
      const auto points_str = common::splitValues(line, '@');
      auto positions = std::vector<Position>{};
      positions.reserve(points_str.size());
      for (const auto &point_str : points_str) {
        const auto point = common::toVecUInt(point_str);
        positions.emplace_back(point[0], point[1]);
      }
      for (auto i = 1u; i < positions.size(); ++i) {
        auto range = Range2D{};
        range.update(positions[i - 1]);
        range.update(positions[i]);
        if (range.x.min == range.x.max) {
          for (auto y = range.y.min; y <= range.y.max; ++y)
            addCell(Position{positions[i].x, y}, Type::ROCK);
        } else if (range.y.min == range.y.max) {
          for (auto x = range.x.min; x <= range.x.max; ++x)
            addCell(Position{x, positions[i].y}, Type::ROCK);
        } else {
          throw std::invalid_argument("misaligned path");
        }
      }
    }
    m_floor_level = m_range.y.max + 2;
  }

  UInt nbSands() const { return m_nb_sands; }

  bool addCell(const Position &position, Type type) {
    if (m_cells.insert({position, type}).second) {
      m_range.update(position);
      if (type == Type::SAND)
        ++m_nb_sands;
      return true;
    }
    return false;
  }

  bool addSandGrain(bool with_floor) {
    auto grain = sand_source;
    const auto y_max = with_floor ? m_floor_level - 1 : m_floor_level - 2;
    while (grain.y < y_max) {
      auto it = m_cells.find(Position{grain.x, grain.y + 1});
      if (it != std::end(m_cells)) {
        it = m_cells.find(Position{grain.x - 1, grain.y + 1});
        if (it == std::end(m_cells)) {
          --grain.x;
        } else {
          it = m_cells.find(Position{grain.x + 1, grain.y + 1});
          if (it == std::end(m_cells)) {
            ++grain.x;
          } else {
            if (grain == sand_source) {
              if (with_floor) {
                addCell(grain, Type::SAND);
                return false;
              }
              throw std::invalid_argument("grain cannot leave source");
            }
            if (not addCell(grain, Type::SAND))
              throw std::invalid_argument("failed to add grain");
            return true;
          }
        }
      }
      ++grain.y;
    }
    if (with_floor) {
      if (grain.y == m_floor_level - 1) {
        if (not addCell(grain, Type::SAND))
          throw std::invalid_argument("failed to add grain at floor level");
        return true;
      } else {
        throw std::invalid_argument("grain is not at floor level");
      }
    }
    return false;
  }

  void fill(bool with_floor) {
    while (addSandGrain(with_floor)) {
    }
  }

  QString toString() const {
    const auto with_floor = m_range.y.max == m_floor_level - 1;
    auto res = QString("");
    auto range = m_range;
    if (with_floor) {
      --range.x.min;
      ++range.x.max;
    }
    for (Int y = range.y.min; y <= range.y.max; ++y) {
      for (Int x = range.x.min; x <= range.x.max; ++x) {
        const auto position = Position{x, y};
        const auto it = m_cells.find(position);
        if (it == std::end(m_cells))
          res.push_back(position == sand_source ? '+' : '.');
        else
          res.push_back(it->second == Type::ROCK ? '#' : 'o');
      }
      res.push_back('\n');
    }
    if (with_floor) {
      for (Int x = range.x.min; x <= range.x.max; ++x)
        res.push_back('#');
      res.push_back('\n');
    }
    return res;
  }

private:
  UInt m_nb_sands{0};
  std::map<Position, Type> m_cells;
  Range2D m_range{};
  Int m_floor_level{0};
};

} // namespace puzzle_2022_14

void Solver_2022_14_1::solve(const QString &input) {
  auto cave = puzzle_2022_14::Cave{input};
  cave.fill(false);
  emit output(cave.toString());
  emit finished(QString("%1").arg(cave.nbSands()));
}

void Solver_2022_14_2::solve(const QString &input) {
  auto cave = puzzle_2022_14::Cave{input};
  cave.fill(true);
  emit output(cave.toString());
  emit finished(QString("%1").arg(cave.nbSands()));
}
