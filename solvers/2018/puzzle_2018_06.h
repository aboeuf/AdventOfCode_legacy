#pragma once
#include <solvers/solvers.h>

namespace puzzle_2018_06 {

struct Point : public QPoint {
  Point(int x, int y);
  uint area{0};
};

class VoronoiDiagram {
public:
  VoronoiDiagram(const QString &input);
  const std::vector<Point> &points() const;
  int xMin() const;
  int yMin() const;
  int xMax() const;
  int yMax() const;

  uint largestArea() const;
  uint safeRegionArea() const;
  std::vector<std::size_t> closestPoints(int x, int y) const;
  DisplayData getDisplayData() const;

private:
  std::vector<Point> m_points;
  int m_xmin{std::numeric_limits<int>::max()};
  int m_ymin{std::numeric_limits<int>::max()};
  int m_xmax{std::numeric_limits<int>::min()};
  int m_ymax{std::numeric_limits<int>::min()};
};

} // namespace puzzle_2018_06

class Solver_2018_06_1 : public Solver {
public:
  void solve(const QString &input) override;
};

class Solver_2018_06_2 : public Solver {
public:
  void solve(const QString &input) override;
};
