#include <deque>
#include <solvers/2021/puzzle_2021_09.h>
#include <solvers/common.h>

using Int = unsigned long long int;

struct Location {
  Location() = default;
  Location(const QChar &c) { m_heigth = QString(c).toULongLong(); }

  bool isLowPoint() const {
    for (const auto *neighbor : m_neighbors)
      if (neighbor and neighbor->m_heigth <= m_heigth)
        return false;
    return true;
  }

  Int riskLevel() const { return m_heigth + Int{1}; }

  Int m_heigth{0};
  std::array<Location *, 4> m_neighbors{nullptr, nullptr, nullptr, nullptr};
  Location *m_lowest_neighbor{nullptr};
  Int m_nb_flowing_in{0};
};

class Heightmap {
public:
  Heightmap(const QString &input) {
    const auto lines = common::splitLines(input);
    m_locations.reserve(lines.size());
    for (const auto &line : lines) {
      m_locations.push_back(std::vector<Location>{});
      m_locations.back().reserve(line.size());
      for (const auto &chr : line)
        m_locations.back().emplace_back(chr);
    }
    for (auto i = 0u; i < m_locations.size(); ++i) {
      for (auto j = 0u; j < m_locations[i].size(); ++j) {
        if (i != 0u)
          m_locations[i][j].m_neighbors[0] = &(m_locations[i - 1][j]);
        if (i + 1 < m_locations.size())
          m_locations[i][j].m_neighbors[1] = &(m_locations[i + 1][j]);
        if (j != 0u)
          m_locations[i][j].m_neighbors[2] = &(m_locations[i][j - 1]);
        if (j + 1 < m_locations[i].size())
          m_locations[i][j].m_neighbors[3] = &(m_locations[i][j + 1]);
        auto min_height = Int{10};
        for (auto *n : m_locations[i][j].m_neighbors) {
          if (n and n->m_heigth < min_height) {
            m_locations[i][j].m_lowest_neighbor = n;
            min_height = n->m_heigth;
          }
        }
      }
    }
    for (auto i = 0u; i < m_locations.size(); ++i) {
      for (auto j = 0u; j < m_locations[i].size(); ++j) {
        auto *loc = &(m_locations[i][j]);
        if (loc->m_heigth < Int{9}) {
          while (loc and not loc->isLowPoint())
            loc = loc->m_lowest_neighbor;
          if (loc)
            ++(loc->m_nb_flowing_in);
        }
      }
    }
  }

  QString solvePuzzleOne() const {
    auto sum = Int{0};
    for (auto i = 0u; i < m_locations.size(); ++i)
      for (auto j = 0u; j < m_locations[i].size(); ++j)
        if (m_locations[i][j].m_nb_flowing_in != Int{0})
          sum += m_locations[i][j].riskLevel();
    return QString("%1").arg(sum);
  }

  QString solvePuzzleTwo() const {
    auto bassins = std::deque<const Location *>{};
    for (auto i = 0u; i < m_locations.size(); ++i)
      for (auto j = 0u; j < m_locations[i].size(); ++j)
        if (m_locations[i][j].m_nb_flowing_in != Int{0})
          bassins.push_back(&(m_locations[i][j]));
    std::sort(std::begin(bassins), std::end(bassins),
              [](const Location *lhs, const Location *rhs) {
                return lhs->m_nb_flowing_in > rhs->m_nb_flowing_in;
              });
    auto product = Int{1};
    auto n = 0;
    for (auto it = std::begin(bassins); n < 3 and it != std::end(bassins);
         ++it) {
      product *= (*it)->m_nb_flowing_in;
      ++n;
    }
    return QString("%1").arg(product);
  }

private:
  std::vector<std::vector<Location>> m_locations;
};

void Solver_2021_09_1::solve(const QString &input) {
  emit finished(Heightmap(input).solvePuzzleOne());
}

void Solver_2021_09_2::solve(const QString &input) {
  emit finished(Heightmap(input).solvePuzzleTwo());
}
