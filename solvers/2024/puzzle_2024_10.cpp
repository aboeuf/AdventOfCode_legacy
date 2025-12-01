#include <solvers/2024/puzzle_2024_10.h>
#include <solvers/common.h>
#include <solvers/qpoint_hash.hpp>

namespace puzzle_2024_10 {

using Int = unsigned long long;

struct TrailNode;
using TrailNodePtr = std::shared_ptr<TrailNode>;

struct TrailNode {
  TrailNode(const QPoint &position, Int height,
            const TrailNodePtr &parent = nullptr)
      : position{position}, height{height}, parent{parent} {}

  void getUniqueLeaves(QSet<QPoint> &leaves) const {
    if (height == Int{9}) {
      leaves.insert(position);
      return;
    }
    for (const auto &child : children) {
      child->getUniqueLeaves(leaves);
    }
  }

  void getRaiting(Int &raiting) const {
    if (height == Int{9}) {
      ++raiting;
      return;
    }
    for (const auto &child : children) {
      child->getRaiting(raiting);
    }
  }

  QPoint position;
  Int height;
  TrailNodePtr parent;
  QList<TrailNodePtr> children{};
};

const auto directions =
    QVector<QPoint>{QPoint{-1, 0}, QPoint{1, 0}, QPoint{0, -1}, QPoint{0, 1}};

class Map {
public:
  Map(const QString &input) {
    const auto lines = common::splitLines(input);
    m_nb_rows = lines.size();
    if (m_nb_rows == 0) {
      throw std::invalid_argument{"puzzle_2024_10::Map: empty input"};
    }
    m_elevations.reserve(m_nb_rows);
    m_nb_columns = lines.front().size();
    auto ok = true;
    auto row = 0;
    for (const auto &line : lines) {
      if (lines.front().size() != m_nb_columns) {
        throw std::invalid_argument{
            "puzzle_2024_10::Map: incoherent line size"};
      }
      m_elevations.emplace_back();
      m_elevations.back().reserve(m_nb_columns);
      auto column = 0;
      for (const auto &c : line) {
        if (c == '0') {
          m_trailheads.push_back(
              std::make_shared<TrailNode>(QPoint(row, column), 0));
        }
        m_elevations.back().emplace_back(QString(c).toULongLong(&ok));
        if (not ok) {
          throw std::invalid_argument{
              "puzzle_2024_10::Map: cannot convert char to Int"};
        }
        ++column;
      }
      ++row;
    }
    for (auto trailhead : m_trailheads) {
      develop(trailhead);
    }
  }

  QString solveOne() const {
    auto score = Int{0};
    for (auto trailhead : m_trailheads) {
      auto leaves = QSet<QPoint>{};
      trailhead->getUniqueLeaves(leaves);
      score += static_cast<Int>(leaves.size());
    }
    return QString("%1").arg(score);
  }

  QString solveTwo() const {
    auto raiting = Int{0};
    for (auto trailhead : m_trailheads) {
      trailhead->getRaiting(raiting);
    }
    return QString("%1").arg(raiting);
  }

private:
  bool isInMap(const QPoint &position) const {
    return position.x() >= 0 and position.x() < m_nb_rows and
           position.y() >= 0 and position.y() < m_nb_columns;
  }

  void develop(TrailNodePtr node) {
    for (const auto &direction : directions) {
      const auto child_position = node->position + direction;
      if (isInMap(child_position)) {
        const auto child_height =
            m_elevations[child_position.x()][child_position.y()];
        if (child_height == node->height + 1u) {
          node->children << std::make_shared<TrailNode>(child_position,
                                                        child_height, node);
        }
      }
    }
    for (auto child : node->children) {
      develop(child);
    }
  }

  std::vector<std::vector<Int>> m_elevations;
  int m_nb_rows;
  int m_nb_columns;
  QList<TrailNodePtr> m_trailheads;
};

} // namespace puzzle_2024_10

void Solver_2024_10_1::solve(const QString &input) {
  const auto map = puzzle_2024_10::Map(input);
  emit finished(map.solveOne());
}

void Solver_2024_10_2::solve(const QString &input) {
  const auto map = puzzle_2024_10::Map(input);
  emit finished(map.solveTwo());
}
