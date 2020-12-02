#include <2019/puzzle_2019_3.h>
#include <common.h>
#include <QPointF>
#include <QDebug>

namespace puzzle_2019_3 {

struct VerticalSegment
{
  VerticalSegment(int x, int y1, int y2) :
    x{x},
    y_min{std::min(y1, y2)},
    y_max{std::max(y1, y2)} {}

  int x;
  int y_min;
  int y_max;

  QString toString() {
    return QString("V(%1, %2, %3)").arg(x).arg(y_min).arg(y_max);
  }
};

struct HorizontalSegment
{
  HorizontalSegment(int y, int x1, int x2) :
    y{y},
    x_min{std::min(x1, x2)},
    x_max{std::max(x1, x2)} {}

  bool intersects(const VerticalSegment& seg, QPoint& intersection) const
  {
    if (x_min <= seg.x && seg.x <= x_max && seg.y_min <= y && y <= seg.y_max) {
      intersection.setX(seg.x);
      intersection.setY(y);
      return true;
    }
    return false;
  }

  int y;
  int x_min;
  int x_max;

  QString toString() {
    return QString("H(%1, %2, %3)").arg(y).arg(x_min).arg(x_max);
  }
};

int manhattanNorm(const QPoint& pt)
{
  return std::abs(pt.x()) + std::abs(pt.y());
}

void generateSegments(const QStringList& inputs,
                      std::vector<VerticalSegment>& vertical,
                      std::vector<HorizontalSegment>& horizontal)
{
  vertical.clear();
  horizontal.clear();
  vertical.reserve(inputs.size());
  horizontal.reserve(inputs.size());
  QPoint current(0, 0);
  for (QString input : inputs) {
    if (input.size() < 2)
      continue;
    const QChar dir = input.front();
    input.remove(0, 1);
    bool ok;
    int value = input.toInt(&ok);
    if (!ok)
      continue;
    if (dir == 'R') {
      int x = current.x() + value;
      horizontal.emplace_back(current.y(), current.x(), x);
      current.setX(x);
    } else if (dir == 'L') {
      int x = current.x() - value;
      horizontal.emplace_back(current.y(), x, current.x());
      current.setX(x);
    } else if (dir == 'U') {
      int y = current.y() + value;
      vertical.emplace_back(current.x(), current.y(), y);
      current.setY(y);
    } else if (dir == 'D') {
      int y = current.y() - value;
      vertical.emplace_back(current.x(), y, current.y());
      current.setY(y);
    }
  }
}

void getNearestIntersectionDistance(const std::vector<VerticalSegment>& vertical,
                                    const std::vector<HorizontalSegment>& horizontal,
                                    int& dist_min)
{
  QPoint intersection;
  for (const HorizontalSegment& h : horizontal) {
    for (const VerticalSegment& v : vertical) {
      if (h.intersects(v, intersection)) {
        int dist = manhattanNorm(intersection);
        if (dist != 0 && dist < dist_min)
          dist_min = dist;
      }
    }
  }
}

}

PuzzleSolver Puzzle_2019_3::solver_1 = [](const QString& input) {
  using namespace puzzle_2019_3;
  const QStringList lines = common::splitLines(input);
  if (lines.size() < 2)
    return QString("Bad number of lines (%1)").arg(lines.size());
  std::vector<VerticalSegment> vertical_A, vertical_B;
  std::vector<HorizontalSegment> horizontal_A, horizontal_B;
  generateSegments(common::splitValues(lines[0]), vertical_A, horizontal_A);
  generateSegments(common::splitValues(lines[1]), vertical_B, horizontal_B);
  int dist_min = std::numeric_limits<int>::max();
  getNearestIntersectionDistance(vertical_A, horizontal_B, dist_min);
  getNearestIntersectionDistance(vertical_B, horizontal_A, dist_min);
  if (dist_min == std::numeric_limits<int>::max())
    return QString{"No intersections"};
  return QString::number(dist_min);
};
