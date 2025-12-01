#include <solvers/2021/puzzle_2021_05.h>
#include <solvers/common.h>

struct Point {
  uint m_x{0u};
  uint m_y{0u};
};

class AABB {
public:
  AABB() = default;

  void update(const Point &pt) {
    m_x_min = std::min(m_x_min, pt.m_x);
    m_y_min = std::min(m_y_min, pt.m_y);
    m_x_max = std::max(m_x_max, pt.m_x);
    m_y_max = std::max(m_y_max, pt.m_y);
  }

  void update(const AABB &aabb) {
    m_x_min = std::min(m_x_min, aabb.xMin());
    m_y_min = std::min(m_y_min, aabb.yMin());
    m_x_max = std::max(m_x_max, aabb.xMax());
    m_y_max = std::max(m_y_max, aabb.yMax());
  }

  bool isInside(uint x, uint y) const {
    return m_x_min <= x and m_y_min <= y and x <= m_x_max and y <= m_y_max;
  }

  uint xMin() const { return m_x_min; }
  uint yMin() const { return m_y_min; }
  uint xMax() const { return m_x_max; }
  uint yMax() const { return m_y_max; }

private:
  uint m_x_min{std::numeric_limits<uint>::max()};
  uint m_y_min{std::numeric_limits<uint>::max()};
  uint m_x_max{0u};
  uint m_y_max{0u};
};

class Line {
public:
  Line(const QString &input) {
    auto rx = QRegExp{"^(\\d+),(\\d+) \\-> (\\d+),(\\d+)$"};
    if (not rx.exactMatch(input)) {
      m_error = "Invalid input: " + input;
      return;
    }
    m_first.m_x = rx.cap(1).toUInt();
    m_first.m_y = rx.cap(2).toUInt();
    m_second.m_x = rx.cap(3).toUInt();
    m_second.m_y = rx.cap(4).toUInt();
    m_aabb.update(m_first);
    m_aabb.update(m_second);
  }

  const AABB &aabb() const { return m_aabb; }
  const QString &error() const { return m_error; }
  bool isHV() const {
    return m_first.m_y == m_second.m_y or m_first.m_x == m_second.m_x;
  }
  bool isInAABB(uint x, uint y) const { return m_aabb.isInside(x, y); }

  bool isIn(uint x, uint y) const {
    if (not isInAABB(x, y))
      return false;
    if (isHV())
      return true;
    if ((m_first.m_x == m_aabb.xMin() and m_first.m_y == m_aabb.yMin()) or
        (m_second.m_x == m_aabb.xMin() and m_second.m_y == m_aabb.yMin()))
      return x - m_aabb.xMin() == y - m_aabb.yMin();
    return x - m_aabb.xMin() == m_aabb.yMax() - y;
  }

private:
  Point m_first;
  Point m_second;
  AABB m_aabb;
  QString m_error{""};
};

class Vents {
public:
  Vents(const QString &input) {
    const auto lines = common::splitLines(input);
    if (lines.isEmpty()) {
      m_error = "Empty";
      return;
    }
    m_lines.reserve(lines.size());
    for (const auto &line : lines) {
      m_lines.emplace_back(line);
      if (not m_lines.back().error().isEmpty()) {
        m_error = m_lines.back().error();
        return;
      }
      m_aabb.update(m_lines.back().aabb());
    }
  }

  const QString &error() const { return m_error; }

  QString solvePuzzleOne() const {
    auto hv = std::vector<const Line *>{};
    hv.reserve(m_lines.size());
    for (const auto &line : m_lines)
      if (line.isHV())
        hv.emplace_back(&line);
    auto nb_points_total = 0u;
    for (auto x = m_aabb.xMin(); x <= m_aabb.xMax(); ++x) {
      for (auto y = m_aabb.yMin(); y <= m_aabb.yMax(); ++y) {
        auto nb_points = 0u;
        for (auto i = 0u; nb_points < 2 and i < hv.size(); ++i) {
          if (hv[i]->isInAABB(x, y))
            ++nb_points;
        }
        if (nb_points > 1)
          ++nb_points_total;
      }
    }
    return QString{"%1"}.arg(nb_points_total);
  }

  QString solvePuzzleTwo() const {
    auto nb_points_total = 0u;
    for (auto x = m_aabb.xMin(); x <= m_aabb.xMax(); ++x) {
      for (auto y = m_aabb.yMin(); y <= m_aabb.yMax(); ++y) {
        auto nb_points = 0u;
        for (auto i = 0u; nb_points < 2 and i < m_lines.size(); ++i) {
          if (m_lines[i].isIn(x, y))
            ++nb_points;
        }
        if (nb_points > 1)
          ++nb_points_total;
      }
    }
    return QString{"%1"}.arg(nb_points_total);
  }

private:
  QString m_error{""};
  std::vector<Line> m_lines;
  AABB m_aabb;
};

void Solver_2021_05_1::solve(const QString &input) {
  const auto vents = Vents{input};
  if (not vents.error().isEmpty()) {
    emit finished(vents.error());
    return;
  }
  emit finished(vents.solvePuzzleOne());
}

void Solver_2021_05_2::solve(const QString &input) {
  const auto vents = Vents{input};
  if (not vents.error().isEmpty()) {
    emit finished(vents.error());
    return;
  }
  emit finished(vents.solvePuzzleTwo());
}
