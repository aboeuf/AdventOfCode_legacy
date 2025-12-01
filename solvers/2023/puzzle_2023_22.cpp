#include <QRect>
#include <solvers/common.h>
#include <solvers/2023/puzzle_2023_22.h>

namespace puzzle_2023_22 {

struct Coordinates {
  Coordinates(const QString &input) {
    const auto splitted = common::splitValues(input, ',');
    if (splitted.size() != 3)
      common::throwInvalidArgumentError(
          "Coordinates::constructor: invalid split size");
    auto ok = true;
    x = splitted[0].toInt(&ok);
    if (not ok)
      common::throwInvalidArgumentError(
          QString("Coordinates::constructor: cannot cast x value '%1' to int")
              .arg(splitted[0]));
    y = splitted[1].toInt(&ok);
    if (not ok)
      common::throwInvalidArgumentError(
          QString("Coordinates::constructor: cannot cast y value '%1' to int")
              .arg(splitted[1]));
    z = splitted[2].toInt(&ok);
    if (not ok)
      common::throwInvalidArgumentError(
          QString("Coordinates::constructor: cannot cast z value '%1' to int")
              .arg(splitted[2]));
  }
  int x;
  int y;
  int z;
};

inline int diff(const int start, const int stop) { return stop - start + 1; }

class Brick {
public:
  Brick(const QString &input)
      : m_is_at_rest{false}, m_interseting_bricks{}, m_parents{}, m_children{} {
    const auto splitted = common::splitValues(input, '~');
    if (splitted.size() != 2)
      common::throwInvalidArgumentError(
          "Brick::constructor: invalid split size");
    const auto start = Coordinates{splitted[0]};
    const auto stop = Coordinates{splitted[1]};
    const auto z_min = std::min(start.z, stop.z);
    if (z_min < 1)
      common::throwInvalidArgumentError(
          "Brick::constructor: z min is bellow threshold");
    m_z_min = static_cast<uint>(z_min);
    m_height = static_cast<uint>(std::abs(diff(start.z, stop.z)));
    m_xy =
        QRect{start.x, start.y, diff(start.x, stop.x), diff(start.y, stop.y)};
  }

  bool isAtRest() const { return m_is_at_rest; }
  const QSet<std::size_t> &intersetingBricks() const {
    return m_interseting_bricks;
  }
  const QSet<std::size_t> &parents() const { return m_parents; }
  const QSet<std::size_t> &children() const { return m_children; }
  const QRect &xy() const { return m_xy; }
  uint zMin() const { return m_z_min; }
  uint zMax() const { return m_z_min + m_height; }
  uint height() const { return m_height; }

  void reset() {
    m_is_at_rest = false;
    m_interseting_bricks.clear();
    resetParentsAndChildren();
  }

  bool intersectsXY(const std::size_t brick_index) const {
    return m_interseting_bricks.contains(brick_index);
  }

  void addIntersetion(const std::size_t brick_index) {
    m_interseting_bricks << brick_index;
  }

  void resetParentsAndChildren() {
    m_parents.clear();
    m_children.clear();
  }

  void addParent(const std::size_t brick_index) { m_parents << brick_index; }

  void addChild(const std::size_t brick_index) { m_children << brick_index; }

  bool settleAt(const uint z) {
    m_is_at_rest = true;
    if (m_z_min > z) {
      m_z_min = z;
      return true;
    }
    return false;
  }

private:
  bool m_is_at_rest;
  QSet<std::size_t> m_interseting_bricks;
  QSet<std::size_t> m_parents;
  QSet<std::size_t> m_children;
  QRect m_xy;
  uint m_z_min;
  uint m_height;
};

class Stack {
public:
  Stack(const QString &input) {
    const auto lines = common::splitLines(input);
    m_bricks.reserve(lines.size());
    for (const auto &line : lines)
      m_bricks.emplace_back(line);
    computeIntersections();
    settle();
  }

  Stack(const Stack &stack, const std::size_t to_remove) : m_bricks{} {
    if (not stack.bricks().empty()) {
      m_bricks.reserve(stack.bricks().size() - 1u);
      for (auto i = 0u; i < stack.bricks().size(); ++i) {
        if (i != to_remove)
          m_bricks.emplace_back(stack.bricks().at(i));
      }
    }
    for (auto &brick : m_bricks)
      brick.reset();
    computeIntersections();
  }

  const std::vector<Brick> &bricks() const { return m_bricks; }

  uint settle() {
    uint nb_falling = 0u;
    auto it = findMinUnsettled();
    while (it != std::end(m_bricks)) {
      if (it->settleAt(findMinRestingZ(*it)))
        ++nb_falling;
      it = findMinUnsettled();
    }
    return nb_falling;
  }

  QString solveOne() {
    const auto removable = getRemovableBricks();
    return QString("%1").arg(removable.size());
  }

  QString solveTwo() {
    const auto removable = getRemovableBricks();
    auto sum = 0u;
    for (auto i = 0u; i < m_bricks.size(); ++i)
      if (not removable.contains(i)) {
        auto copy = Stack(*this, i);
        sum += copy.settle();
      }
    return QString("%1").arg(sum);
  }

private:
  std::vector<Brick>::iterator findMinUnsettled() {
    auto res = std::end(m_bricks);
    for (auto it = std::begin(m_bricks); it != std::end(m_bricks); ++it) {
      if (not it->isAtRest()) {
        if (res == std::end(m_bricks) or res->zMin() > it->zMin())
          res = it;
      }
    }
    return res;
  }

  uint findMinRestingZ(const Brick &brick) {
    auto min = 1u;
    for (auto index : brick.intersetingBricks()) {
      if (m_bricks[index].isAtRest())
        min = std::max(min, m_bricks[index].zMax());
    }
    return min;
  }

  void resetParentAndChildren() {
    for (auto &brick : m_bricks)
      brick.resetParentsAndChildren();
  }

  void computeGraph() {
    resetParentAndChildren();
    for (auto i = 0u; i < m_bricks.size(); ++i) {
      for (auto j = 0u; j < m_bricks.size(); ++j) {
        if (i != j) {
          if (m_bricks[i].intersectsXY(j) and
              m_bricks[i].zMax() == m_bricks[j].zMin()) {
            m_bricks[i].addChild(j);
            m_bricks[j].addParent(i);
          }
        }
      }
    }
  }

  QSet<std::size_t> getRemovableBricks() {
    computeGraph();
    auto removable = QSet<std::size_t>{};
    for (auto i = 0u; i < m_bricks.size(); ++i) {
      auto is_removable = true;
      for (auto j : m_bricks[i].children()) {
        if (m_bricks[j].parents().size() < 2) {
          is_removable = false;
          break;
        }
      }
      if (is_removable)
        removable << i;
    }
    return removable;
  }

  void computeIntersections() {
    for (auto i = 0u; i < m_bricks.size(); ++i) {
      for (auto j = i + 1u; j < m_bricks.size(); ++j) {
        if (m_bricks[i].xy().intersects(m_bricks[j].xy())) {
          m_bricks[i].addIntersetion(j);
          m_bricks[j].addIntersetion(i);
        }
      }
    }
  }

  std::vector<Brick> m_bricks;
};

} // namespace puzzle_2023_22

void Solver_2023_22_1::solve(const QString &input) {
  auto stack = puzzle_2023_22::Stack{input};
  emit finished(stack.solveOne());
}

void Solver_2023_22_2::solve(const QString &input) {
  auto stack = puzzle_2023_22::Stack{input};
  emit finished(stack.solveTwo());
}
