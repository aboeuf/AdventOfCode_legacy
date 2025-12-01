#include <solvers/common.h>
#include <limits>
#include <solvers/2023/puzzle_2023_21.h>

#include <QFile>
#include <QTextStream>

namespace puzzle_2023_21 {

using Int = unsigned long long int;

enum class CellType { plot, rock };

enum class Direction { north, west, south, east };

const auto directions = std::array<Direction, 4>{
    Direction::north, Direction::west, Direction::south, Direction::east};

struct Coordinates {
  Coordinates() = default;
  Coordinates(int x, int y) : x{x}, y{y} {}
  Coordinates(const Coordinates &previous, const Direction next)
      : Coordinates{previous} {
    if (next == Direction::north)
      --x;
    else if (next == Direction::west)
      --y;
    else if (next == Direction::south)
      ++x;
    else if (next == Direction::east)
      ++y;
    else
      common::throwInvalidArgumentError(
          "Coordinates::constructor: unknown direction");
  }
  int x{0};
  int y{0};
};

inline void exportCosMap(const QHash<Coordinates, Int> &cost_map) {
  QFile file("/home/aboeuf/cost_map.csv");
  if (not file.open(QIODevice::WriteOnly | QIODevice::Text))
    return;
  auto x_min = std::numeric_limits<int>::max();
  auto y_min = std::numeric_limits<int>::max();
  auto x_max = std::numeric_limits<int>::min();
  auto y_max = std::numeric_limits<int>::min();
  for (auto it = std::begin(cost_map); it != std::end(cost_map); ++it) {
    x_min = std::min(x_min, it.key().x);
    y_min = std::min(y_min, it.key().y);
    x_max = std::max(x_max, it.key().x);
    y_max = std::max(y_max, it.key().y);
  }
  QTextStream out(&file);
  out << "y";
  for (auto x = x_min; x <= x_max; ++x)
    out << QString(",%1").arg(x);
  out << "\n";
  for (auto y = y_max; y >= y_min; --y) {
    out << QString("%1").arg(y);
    for (auto x = x_min; x <= x_max; ++x) {
      out << ",";
      const auto c = Coordinates(x, y);
      if (cost_map.contains(c)) {
        out << QString("%1").arg(cost_map[c]);
      } else {
        out << "#";
      }
    }
    out << "\n";
  }
  file.close();
}

bool operator==(const Coordinates &lhs, const Coordinates &rhs) {
  return lhs.x == rhs.x and lhs.y == rhs.y;
}

Int qHash(const Coordinates &c) {
  return qHash(QString("%1,%2").arg(c.x).arg(c.y));
}

class Grid {
public:
  Grid(const QString &input) {
    const auto lines = common::splitLines(input);
    m_dimension = lines.size();
    if (m_dimension == 0)
      common::throwInvalidArgumentError("Grid::constructor: empty grid");
    m_cells.reserve(m_dimension);
    auto start_set = false;
    auto x = 0;
    for (const auto &line : lines) {
      if (line.size() != m_dimension)
        common::throwInvalidArgumentError(
            "Grid::constructor: incoherent width");
      m_cells.emplace_back();
      m_cells.back().reserve(m_dimension);
      auto y = 0;
      for (const auto &c : line) {
        if (c == '.') {
          m_cells.back().emplace_back(CellType::plot);
        } else if (c == '#') {
          m_cells.back().emplace_back(CellType::rock);
        } else if (c == 'S') {
          if (start_set)
            common::throwInvalidArgumentError(
                "Grid::constructor: more than one starting point is defined");
          m_cells.back().emplace_back(CellType::plot);
          m_start = Coordinates{x, y};
          start_set = true;
        } else {
          common::throwInvalidArgumentError(
              QString("Grid::constructor: unknown cell type '%1'").arg(c));
        }
        ++y;
      }
      ++x;
    }
    if (not start_set)
      common::throwInvalidArgumentError(
          "Grid::constructor: no starting point has been defined");
  }

  Int dimension() const { return static_cast<Int>(m_dimension); }

  Coordinates toLocal(const Coordinates &c) const {
    const auto mod = [](const int value, const int modulo) {
      if (value < 0) {
        const auto remainder = abs(value) % modulo;
        return remainder == 0 ? 0 : modulo - remainder;
      }
      return value % modulo;
    };
    return Coordinates(mod(c.x, m_dimension), mod(c.y, m_dimension));
  }

  CellType getInfinite(const Coordinates &c) const {
    const auto local = toLocal(c);
    return m_cells[local.x][local.y];
  }

  bool isPlotCell(const Coordinates &c, bool v1) const {
    if (v1)
      return c.x > -1 and c.x < m_dimension and c.y > -1 and
             c.y < m_dimension and m_cells[c.x][c.y] == CellType::plot;
    return getInfinite(c) == CellType::plot;
  }

  std::vector<Int> getNbCellsPerSteps(const Int nb_steps_max) const {
    const auto cost_map = computeCostMap(nb_steps_max);
    auto nb_cells = std::vector<Int>{};
    nb_cells.resize(nb_steps_max + 1, Int{0});
    for (auto it = std::begin(cost_map); it != std::end(cost_map); ++it)
      ++nb_cells[it.value()];
    return nb_cells;
  }

  QHash<Coordinates, Int> computeCostMap(const Int nb_steps_max) const {
    auto cost_map = QHash<Coordinates, Int>{};
    cost_map[m_start] = Int{0};
    auto open_set = common::OpenSet<Coordinates, Int>{};
    open_set.push(m_start, 0);
    auto current = open_set.pop();
    while (current.has_value()) {
      const auto &father = current->first;
      if (not cost_map.contains(father))
        common::throwRunTimeError("computeCostMap: cannot find father cost");
      const auto father_cost = cost_map[father];
      if (father_cost < nb_steps_max) {
        const auto child_cost = father_cost + Int{1};
        for (const auto direction : directions) {
          const auto child = Coordinates(father, direction);
          if (isPlotCell(child, false)) {
            if (not cost_map.contains(child) or child_cost < cost_map[child]) {
              cost_map[child] = child_cost;
              open_set.push(child, child_cost);
            }
          }
        }
      }
      current = open_set.pop();
    }
    return cost_map;
  }

  std::vector<Int> getFirstOrder(const Int N) const {
    const auto dim = static_cast<std::size_t>(m_dimension);
    auto nb_cells = std::vector<Int>{};
    nb_cells.resize(N * dim);
    const auto cost_map = computeCostMap(N);
    for (auto it = std::begin(cost_map); it != std::end(cost_map); ++it)
      if (it.value() < N * dim)
        ++nb_cells[it.value()];
    return nb_cells;
  }

  std::vector<Int> getSecondOrder(const Int N) const {
    const auto dim = static_cast<std::size_t>(m_dimension);
    const auto first_order = getFirstOrder(N);
    auto second_order = std::vector<Int>{};
    second_order.reserve(first_order.size() - dim);
    for (auto i = 4u * dim + 1u; i < first_order.size(); ++i) {
      second_order.emplace_back(first_order[i] - first_order[i - dim]);
    }
    return second_order;
  }

  std::vector<Int> getThirdOrder(const Int N) const {
    const auto dim = static_cast<std::size_t>(m_dimension);
    const auto second_order = getSecondOrder(N);
    auto third_order = std::vector<Int>{};
    third_order.reserve(second_order.size() - dim);
    for (auto i = dim; i < second_order.size(); ++i) {
      third_order.emplace_back(second_order[i] - second_order[i - dim]);
    }
    return third_order;
  }

  std::vector<Int> getThirdOrderTest(const Int N) const {
    const auto dim = static_cast<std::size_t>(m_dimension);
    const auto first_order = getFirstOrder(N);
    auto third_order = std::vector<Int>{};
    third_order.reserve((N - 3u) * dim + 1u);
    for (auto i = 3u * dim; i < N * dim; ++i)
      third_order.emplace_back(first_order[i] + first_order[i - 2u * dim] -
                               2u * first_order[i - dim]);
    return third_order;
  }

  Int getNbCellsTemp(const Int nb_steps_max) const {
    auto cost_map = QHash<Coordinates, Int>{};
    cost_map[m_start] = Int{0};
    auto open_set = common::OpenSet<Coordinates, Int>{};
    open_set.push(m_start, 0);
    auto current = open_set.pop();
    while (current.has_value()) {
      const auto &father = current->first;
      if (not cost_map.contains(father))
        common::throwRunTimeError("cannot find father cost");
      const auto father_cost = cost_map[father];
      if (father_cost < nb_steps_max) {
        const auto child_cost = father_cost + Int{1};
        for (const auto direction : directions) {
          const auto child = Coordinates(father, direction);
          if (isPlotCell(child, false)) {
            if (not cost_map.contains(child) or child_cost < cost_map[child]) {
              cost_map[child] = child_cost;
              open_set.push(child, child_cost);
            }
          }
        }
      }
      current = open_set.pop();
    }
    const auto target_parity = (nb_steps_max % 2u == 0u);
    auto nb_cells = Int{0};
    for (auto it = std::begin(cost_map); it != std::end(cost_map); ++it) {
      const auto parity = (it.value() % 2u == 0u);
      if (parity == target_parity)
        ++nb_cells;
    }
    return nb_cells;
  }

  void test1(const int n, const int m) const {
    const auto L = (2 * n + 1) * m_dimension;
    const auto W = (2 * m + 1) * m_dimension;
    const auto small_to_big = [n, m, this](const Coordinates &c) {
      return Coordinates{c.x + n * m_dimension, c.y + m * m_dimension};
    };
    const auto big_to_small = [n, m, this](const Coordinates &c) {
      return Coordinates{c.x - n * m_dimension, c.y - m * m_dimension};
    };
    const auto valid = [L, W, &small_to_big](const Coordinates &c) {
      const auto converted = small_to_big(c);
      return converted.x > -1 and converted.x < L and converted.y > -1 and
             converted.y < W;
    };
    auto cost_max = Int{0};
    auto cost_map = std::vector<std::vector<Int>>{};
    cost_map.resize(L);
    for (auto i = 0; i < L; ++i)
      cost_map[i].resize(W, std::numeric_limits<Int>::max());
    const auto cost = [&cost_map, &small_to_big,
                       this](const Coordinates &c) -> Int & {
      const auto converted = small_to_big(c);
      return cost_map[converted.x][converted.y];
    };
    cost(m_start) = Int{0};
    auto open_set = common::OpenSet<Coordinates, Int>{};
    open_set.push(m_start, 0);
    auto current = open_set.pop();
    while (current.has_value()) {
      const auto &father = current->first;
      const auto child_cost = cost(father) + Int{1};
      for (const auto direction : directions) {
        const auto child = Coordinates(father, direction);
        if (valid(child) and isPlotCell(child, false)) {
          if (child_cost < cost(child)) {
            cost(child) = child_cost;
            open_set.push(child, child_cost);
            cost_max = std::max(cost_max, child_cost);
          }
        }
      }
      current = open_set.pop();
    }

    QFile file("/home/aboeuf/color_map.csv");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
      QTextStream out(&file);
      for (auto i = 0; i < L; ++i) {
        for (auto j = 0; j < W; ++j) {
          const auto c = Coordinates(i, j);
          const auto converted = big_to_small(c);
          if (isPlotCell(converted, false))
            out << i << "," << j << "," << cost_map[i][j] << "\n";
        }
      }
      file.close();
    }
  }

private:
  std::vector<std::vector<CellType>> m_cells;
  int m_dimension;
  Coordinates m_start;
};

} // namespace puzzle_2023_21

void Solver_2023_21_1::solve(const QString &input) {
  const auto grid = puzzle_2023_21::Grid{input};
  const auto cost_map = grid.computeCostMap(50);
  puzzle_2023_21::exportCosMap(cost_map);
  emit finished("done test1");
}

inline void exportVector(const QString &filepath,
                         const std::vector<puzzle_2023_21::Int> &data) {
  QFile file(filepath);
  if (not file.open(QIODevice::WriteOnly | QIODevice::Text))
    return;
  QTextStream out(&file);
  out << "nb_steps,data\n";
  for (auto i = 0u; i < data.size(); ++i)
    out << i << "," << data[i] << "\n";
  file.close();
}

void Solver_2023_21_2::solve(const QString &input) {
  const auto grid = puzzle_2023_21::Grid{input};
  const auto N = puzzle_2023_21::Int{26501365};
  const auto D = grid.dimension();
  const auto r = N % D;
  const auto q = N / D;
  const auto v_serie = grid.getNbCellsPerSteps(2 * D + r);
  auto u_serie = std::vector<puzzle_2023_21::Int>{};
  u_serie.reserve(v_serie.size());
  for (auto i = 0u; i < v_serie.size(); ++i) {
    const auto v_i = v_serie[i];
    const auto u_i = i > 1u ? v_i + u_serie[i - 2u] : v_i;
    u_serie.emplace_back(u_i);
  }
  auto u_prev = u_serie[D + r];
  auto u_prev_prev = u_serie[r];
  const auto R = u_serie[2 * D + r] - 2 * u_prev + u_prev_prev;
  for (auto i = 0u; i + 1 < q; ++i) {
    const auto u = 2 * u_prev + R - u_prev_prev;
    u_prev_prev = u_prev;
    u_prev = u;
  }
  emit finished(QString("%1").arg(u_prev));
}
