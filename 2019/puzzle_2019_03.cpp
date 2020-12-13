#include <2019/puzzle_2019_03.h>
#include <common.h>
#include <QPoint>
#include <QDebug>
#include <list>

namespace puzzle_2019_03 {

using Wire = std::vector<QPoint>;

void getWire(const QString& input, Wire& wire) {
  wire.clear();
  const QStringList moves = common::splitValues(input);
  wire.reserve(input.size() + 1);
  wire.emplace_back(0, 0);
  for (QString move : moves) {
    if (move.size() < 2)
      continue;
    const QChar dir = move.front();
    move.remove(0, 1);
    bool ok;
    int value = move.toInt(&ok);
    if (!ok)
      continue;
    if (dir == 'R')
      wire.emplace_back(wire.back().x() + value, wire.back().y());
    else if (dir == 'L')
      wire.emplace_back(wire.back().x() - value, wire.back().y());
    else if (dir == 'U')
      wire.emplace_back(wire.back().x(), wire.back().y() + value);
    else if (dir == 'D')
      wire.emplace_back(wire.back().x(), wire.back().y() - value);
  }
}

void getWires(const QString& input, Wire& wire_A, Wire& wire_B)
{
  const QStringList lines = common::splitLines(input);
  if (lines.size() > 0)
    getWire(lines[0], wire_A);
  if (lines.size() > 1)
    getWire(lines[1], wire_B);
}

int manhattanNorm(const QPoint& pt)
{
  return std::abs(pt.x()) + std::abs(pt.y());
}

int manhattanDist(const QPoint& A, const QPoint& B)
{
  return std::abs(A.x() - B.x()) + std::abs(A.y() - B.y());
}

bool computeIntersection(const QPoint& beg_A, const QPoint& end_A,
                         const QPoint& beg_B, const QPoint& end_B,
                         QPoint& intersection)
{
  int min_A_x = std::min(beg_A.x(), end_A.x());
  int max_A_x = std::max(beg_A.x(), end_A.x());
  int min_A_y = std::min(beg_A.y(), end_A.y());
  int max_A_y = std::max(beg_A.y(), end_A.y());
  int min_B_x = std::min(beg_B.x(), end_B.x());
  int max_B_x = std::max(beg_B.x(), end_B.x());
  int min_B_y = std::min(beg_B.y(), end_B.y());
  int max_B_y = std::max(beg_B.y(), end_B.y());

  if (min_A_x == max_A_x && min_B_y == max_B_y) {
    if (min_A_x == 0 && min_B_y == 0)
      return false;
    if (min_B_x <= min_A_x && min_A_x <= max_B_x &&
        min_A_y <= min_B_y && min_B_y <= max_A_y) {
      intersection.setX(min_A_x);
      intersection.setY(min_B_y);
      return true;
    }
    return false;
  }

  if (min_B_x == max_B_x && min_A_y == max_A_y) {
    if (min_B_x == 0 && min_A_y == 0)
      return false;
    if (min_A_x <= min_B_x && min_B_x <= max_A_x &&
        min_B_y <= min_A_y && min_A_y <= max_B_y) {
      intersection.setX(min_B_x);
      intersection.setY(min_A_y);
      return true;
    }
    return false;
  }

  return false;
}

int getNearestIntersection(const Wire& wire_A, const Wire& wire_B)
{
  int min_dist = std::numeric_limits<int>::max();
  QPoint intersection;
  for (auto end_A = std::next(wire_A.begin()); end_A != wire_A.end(); ++end_A) {
    for (auto end_B = std::next(wire_B.begin()); end_B != wire_B.end(); ++end_B) {
      if (computeIntersection(*std::prev(end_A), *end_A, *std::prev(end_B), *end_B, intersection)) {
        int dist = manhattanNorm(intersection);
        if (dist < min_dist)
          min_dist = dist;
      }
    }
  }
  return min_dist;
}

int getMinimumDelay(const Wire& wire_A, const Wire& wire_B)
{
  int min_delay = std::numeric_limits<int>::max();
  QPoint intersection;
  int delay_a = 0;
  for (auto end_A = std::next(wire_A.begin()); end_A != wire_A.end(); ++end_A) {
    int delay_b = 0;
    for (auto end_B = std::next(wire_B.begin()); end_B != wire_B.end(); ++end_B) {
      if (computeIntersection(*std::prev(end_A), *end_A, *std::prev(end_B), *end_B, intersection)) {
        int delay = delay_a + delay_b +
            manhattanDist(*std::prev(end_A), intersection) +
            manhattanDist(*std::prev(end_B), intersection);
        if (delay < min_delay)
          min_delay = delay;
      }
      delay_b += manhattanDist(*std::prev(end_B), *end_B);
    }
    delay_a += manhattanDist(*std::prev(end_A), *end_A);
  }
  return min_delay;
}

}

void Solver_2019_03_1::solve(const QString& input)
{
  using namespace puzzle_2019_03;
  Wire wire_A, wire_B;
  getWires(input, wire_A, wire_B);
  emit finished(QString::number(getNearestIntersection(wire_A, wire_B)));
}

void Solver_2019_03_2::solve(const QString& input)
{
  using namespace puzzle_2019_03;
  Wire wire_A, wire_B;
  getWires(input, wire_A, wire_B);
  emit finished(QString::number(getMinimumDelay(wire_A, wire_B)));
}
