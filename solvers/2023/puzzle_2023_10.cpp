#include <solvers/common.h>
#include <set>
#include <solvers/2023/puzzle_2023_10.h>

namespace puzzle_2023_10 {

struct CellIndex {
  CellIndex() = default;
  CellIndex(int x, int y) : x{x}, y{y} {}

  QString toString() const { return QString("(%1, %2)").arg(x).arg(y); }

  int x{0};
  int y{0};
};

bool operator<(const CellIndex &lhs, const CellIndex &rhs) {
  if (lhs.x == rhs.x)
    return lhs.y < rhs.y;
  return lhs.x < rhs.x;
}

bool operator!=(const CellIndex &lhs, const CellIndex &rhs) {
  return lhs.x != rhs.x or lhs.y != rhs.y;
}

class Cell {
public:
  Cell(const CellIndex &max, const QChar &label, int x, int y)
      : m_label{label}, m_index{x, y}, m_neighbors{} {
    if (x > 0 and
        (m_label == 'S' or m_label == '|' or m_label == 'L' or m_label == 'J'))
      m_neighbors.insert(CellIndex{x - 1, y});
    if (x + 1 < max.x and
        (m_label == 'S' or m_label == '|' or m_label == '7' or m_label == 'F'))
      m_neighbors.insert(CellIndex{x + 1, y});
    if (y > 0 and
        (m_label == 'S' or m_label == '-' or m_label == 'J' or m_label == '7'))
      m_neighbors.insert(CellIndex{x, y - 1});
    if (y + 1 < max.y and
        (m_label == 'S' or m_label == '-' or m_label == 'L' or m_label == 'F'))
      m_neighbors.insert(CellIndex{x, y + 1});
  }

  const QChar &label() const { return m_label; }
  const CellIndex &index() const { return m_index; }
  const std::set<CellIndex> &neighbors() const { return m_neighbors; }

  void pruneNeighbors(
      const std::function<const std::set<CellIndex> &(const CellIndex &)>
          &get_neighbors) {
    for (auto it = std::begin(m_neighbors); it != std::end(m_neighbors);) {
      const auto &neighbors = get_neighbors(*it);
      if (neighbors.find(m_index) == neighbors.end())
        it = m_neighbors.erase(it);
      else
        ++it;
    }
  }

private:
  QChar m_label;
  CellIndex m_index;
  std::set<CellIndex> m_neighbors;
};

struct Probe : public QList<CellIndex> {
  Probe() : QList<CellIndex>{} {}

  Probe(const CellIndex &start) : QList<CellIndex>{} { *this << start; }

  Probe(const Probe &previous, const CellIndex &next) : Probe{previous} {
    *this << next;
  }

  CellIndex current() const { return empty() ? CellIndex{-1, -1} : back(); }

  CellIndex previous() const {
    auto it = std::rbegin(*this);
    if (it == std::rend(*this))
      return CellIndex{-1, -1};
    ++it;
    if (it == std::rend(*this))
      return CellIndex{-1, -1};
    return *it;
  }

  QString toString() const {
    auto full = QString("[");
    for (auto it = std::begin(*this); it != std::end(*this); ++it) {
      full += it->toString();
      if (std::next(it) != std::end(*this))
        full += ", ";
    }
    return QString("current: %1 | previous: %2 | full: %3")
        .arg(current().toString())
        .arg(previous().toString())
        .arg(full);
  }
};

bool operator<(const Probe &lhs, const Probe &rhs) {
  return lhs.current() < rhs.current();
}

class Map {
public:
  Map(const QString &input) {
    const auto lines = common::splitLines(input);
    m_cells.reserve(lines.size());
    m_max.x = lines.size();
    auto x = 0;
    for (const auto &line : lines) {
      m_cells.emplace_back();
      if (m_max.y < 0) {
        m_max.y = line.size();
      } else if (m_max.y != line.size()) {
        common::throwInvalidArgumentError("incoherent y size");
      }
      auto y = 0;
      for (const auto &label : line) {
        m_cells.back().emplace_back(m_max, label, x, y);
        if (m_cells.back().back().label() == 'S')
          m_start = CellIndex{x, y};
        ++y;
      }
      ++x;
    }
    const auto get_neighbors =
        [this](const CellIndex &index) -> const std::set<CellIndex> & {
      return m_cells[index.x][index.y].neighbors();
    };
    for (auto x = 0; x < m_max.x; ++x)
      for (auto y = 0; y < m_max.y; ++y)
        m_cells[x][y].pruneNeighbors(get_neighbors);
    computeLoop();
  }

  int perimeter() const { return m_loop.size() - 1; }

  int area() const {
    auto area = 0;
    for (auto it = std::begin(m_loop); std::next(it) != std::end(m_loop);
         ++it) {
      const auto next = std::next(it);
      area += (it->x * next->y - next->x * it->y);
    }
    return abs(area) / 2;
  }

  QString solveOne() const { return QString("%1").arg(perimeter() / 2); }

  QString solveTwo() const {
    return QString("%1").arg(1 + area() - perimeter() / 2);
  }

private:
  void computeLoop() {
    auto current_probes = std::set<Probe>{Probe{m_start}};
    while (not current_probes.empty()) {
      auto next_probes = std::set<Probe>{};
      for (const auto &current_probe : current_probes) {
        const auto current_index = current_probe.current();
        const auto &cell = m_cells[current_index.x][current_index.y];
        for (const auto &neighbor : cell.neighbors()) {
          if (neighbor != current_probe.previous()) {
            const auto next_probe = Probe{current_probe, neighbor};
            const auto insert_result = next_probes.insert(next_probe);
            if (not insert_result.second) {
              m_loop.reserve(insert_result.first->size() + next_probe.size() -
                             2);
              for (auto &index : *insert_result.first)
                m_loop << index;
              for (auto it = std::next(std::rbegin(next_probe));
                   it != std::rend(next_probe); ++it)
                m_loop << *it;
              return;
            }
          }
        }
      }
      std::swap(current_probes, next_probes);
    }
  }

  std::vector<std::vector<Cell>> m_cells{};
  CellIndex m_start{-1, -1};
  CellIndex m_max{-1, -1};
  QVector<CellIndex> m_loop;
};

} // namespace puzzle_2023_10

void Solver_2023_10_1::solve(const QString &input) {
  const auto map = puzzle_2023_10::Map{input};
  emit finished(map.solveOne());
}

void Solver_2023_10_2::solve(const QString &input) {
  const auto map = puzzle_2023_10::Map{input};
  emit finished(map.solveTwo());
}
