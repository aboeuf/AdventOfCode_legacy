#include <solvers/common.h>
#include <solvers/2023/puzzle_2023_18.h>

namespace puzzle_2023_18 {

using Int = long long int;
using UInt = unsigned long long int;

enum class Direction { up, down, left, right };

inline Direction charToDirV1(const QChar &c) {
  if (c == 'U')
    return Direction::up;
  if (c == 'D')
    return Direction::down;
  if (c == 'R')
    return Direction::right;
  if (c == 'L')
    return Direction::left;
  common::throwInvalidArgumentError(
      QString("charToDirV1: unknown direction '%1'").arg(c));
  return Direction::up;
}

inline Direction charToDirV2(const QChar &c) {
  if (c == '0')
    return Direction::right;
  if (c == '1')
    return Direction::down;
  if (c == '2')
    return Direction::left;
  if (c == '3')
    return Direction::up;
  common::throwInvalidArgumentError(
      QString("charToDirV2: unknown direction '%1'").arg(c));
  return Direction::up;
}

struct Point {
  Point() = default;
  Point(Int x, Int y) : x{x}, y{y} {}
  Point(const Point &start, Direction direction, UInt lenght)
      : x{start.x}, y{start.y} {
    if (direction == Direction::up)
      y += static_cast<Int>(lenght);
    else if (direction == Direction::down)
      y -= static_cast<Int>(lenght);
    else if (direction == Direction::right)
      x += static_cast<Int>(lenght);
    else if (direction == Direction::left)
      x -= static_cast<Int>(lenght);
    else
      common::throwInvalidArgumentError(
          "Point::constructor: unknown direction");
  }

  Int x{0};
  Int y{0};
};

class Edge {
public:
  Edge(const QString &input, const Point &start, bool v1) {
    const auto splitted = common::splitValues(input, ' ');
    if (splitted.size() != 3)
      common::throwInvalidArgumentError(
          QString("Edge::constructor: wrong input size (%1). Should be (3).")
              .arg(splitted.size()));
    if (v1) {
      if (splitted[0].size() != 1)
        common::throwInvalidArgumentError(
            QString(
                "Edge::constructor: wrong direction size (%1). Should be (1).")
                .arg(splitted[0].size()));
      m_direction = charToDirV1(splitted[0].front());
      auto ok = true;
      m_length = static_cast<UInt>(splitted[1].toUInt(&ok));
      if (not ok)
        common::throwInvalidArgumentError(
            QString("Edge::constructor: cannot cast \"%1\" to uint")
                .arg(splitted[1]));
    } else {
      if (splitted[2].size() != 9)
        common::throwInvalidArgumentError(
            QString("Edge::constructor: wrong color size (%1). Should be (9).")
                .arg(splitted[2].size()));
      auto ok = true;
      m_length = static_cast<UInt>(splitted[2].mid(2, 5).toUInt(&ok, 16));
      if (not ok)
        common::throwInvalidArgumentError(
            QString("Edge::constructor: cannot cast hexadecimal \"%1\" to uint")
                .arg(splitted[2].mid(2, 5)));
      m_direction = charToDirV2(splitted[2].at(7));
    }
    m_start = start;
    m_stop = Point(start, m_direction, m_length);
  }

  Direction direction() const { return m_direction; }
  UInt length() const { return m_length; }
  const Point &start() const { return m_start; }
  const Point &stop() const { return m_stop; }

private:
  Direction m_direction;
  UInt m_length;
  Point m_start;
  Point m_stop;
};

class Trench {
public:
  Trench(const QString &input, bool v1) {
    const auto lines = common::splitLines(input);
    m_edges.reserve(lines.size());
    auto edge_start = Point();
    for (const auto &line : lines) {
      m_edges.emplace_back(line, edge_start, v1);
      edge_start = m_edges.back().stop();
    }
  }

  UInt perimeter() const {
    auto sum = UInt{0};
    for (const auto &edge : m_edges)
      sum += edge.length();
    return sum;
  }

  UInt area() const {
    auto signed_area = Int{0};
    for (const auto &edge : m_edges)
      signed_area +=
          (edge.start().x * edge.stop().y - edge.stop().x * edge.start().y);
    return 1u + (static_cast<UInt>(std::abs(signed_area)) + perimeter()) / 2u;
  }

  QString solve() const { return QString("%1").arg(area()); }

private:
  std::vector<Edge> m_edges;
};

} // namespace puzzle_2023_18

void Solver_2023_18_1::solve(const QString &input) {
  const auto trench = puzzle_2023_18::Trench{input, true};
  emit finished(trench.solve());
}

void Solver_2023_18_2::solve(const QString &input) {
  const auto trench = puzzle_2023_18::Trench{input, false};
  emit finished(trench.solve());
}
