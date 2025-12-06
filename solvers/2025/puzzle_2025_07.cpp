#include <solvers/2025/puzzle_2025_07.h>
#include <solvers/common.h>

namespace puzzle_2025_07 {

enum class Type { start, empty, beam, splitter };

using Int = unsigned long long;

struct Cell {
  Cell(Type type) : type{type}, nb_timelines{0} {}
  Type type;
  Int nb_timelines;
};

class TachyonManifold {
public:
  TachyonManifold(const QString &input) {
    const auto lines = common::splitLines(input);
    m_length = lines.size();
    if (m_length == 0) {
      throw std::invalid_argument("empty input");
    }
    m_cells.resize(m_length);
    m_width = lines.front().size();
    auto start_j = -1;
    for (auto i = 0; i < m_length; ++i) {
      const auto &line = lines.at(i);
      if (m_width != line.size()) {
        throw std::invalid_argument("width mismatch");
      }
      for (auto j = 0; j < m_width; ++j) {
        if (line[j] == '.') {
          m_cells[i].emplace_back(Type::empty);
        } else if (line[j] == '^') {
          m_cells[i].emplace_back(Type::splitter);
        } else if (line[j] == 'S') {
          if (i != 0) {
            throw std::invalid_argument("starting point is not on top");
          }
          if (start_j >= 0) {
            throw std::invalid_argument("multiple starting points");
          }
          start_j = j;
          m_cells[i].emplace_back(Type::start);
        } else {
          throw std::invalid_argument("invalid character");
        }
      }
    }
    m_nb_splits = Int{0};
    for (auto i = 0; i + 1 < m_length; ++i) {
        for (auto j = 0; j < m_width; ++j) {
            if (m_cells[i][j].type == Type::start) {
                setBeamAt(i + 1, j, 1);
            } else if (m_cells[i][j].type == Type::beam) {
                const auto nb_timelines = m_cells[i][j].nb_timelines;
                if (m_cells[i + 1][j].type == Type::splitter) {
                    setBeamAt(i + 1, j - 1, nb_timelines);
                    setBeamAt(i + 1, j + 1, nb_timelines);
                    ++m_nb_splits;
                } else {
                    setBeamAt(i + 1, j, nb_timelines);
                }
            }
        }
    }
  }

  QString solveOne() const { return QString("%1").arg(m_nb_splits); }

  QString solveTwo() const {
    auto nb_timelines = Int{0};
    for (auto j = 0; j < m_width; ++j) {
      nb_timelines += m_cells[m_length - 1][j].nb_timelines;
    }
    return QString("%1").arg(nb_timelines);
  }

private:
  void setBeamAt(int i, int j, Int increment) {
    if (i >= 0 and i < m_length and j >= 0 and j < m_width and
        (m_cells[i][j].type == Type::empty or
         m_cells[i][j].type == Type::beam)) {
      m_cells[i][j].type = Type::beam;
      m_cells[i][j].nb_timelines += increment;
    }
  }

  int m_length{-1};
  int m_width{-1};
  std::vector<std::vector<Cell>> m_cells{};
  Int m_nb_splits{0};
};

} // namespace puzzle_2025_07

void Solver_2025_07_1::solve(const QString &input) {
  const auto manifold = puzzle_2025_07::TachyonManifold(input);
  emit finished(manifold.solveOne());
}

void Solver_2025_07_2::solve(const QString &input) {
  const auto manifold = puzzle_2025_07::TachyonManifold(input);
  emit finished(manifold.solveTwo());
}
