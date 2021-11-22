#include <2018/puzzle_2018_06.h>
#include <2018/display/puzzle_2018_06/puzzle_2018_06_display.h>
#include <common.h>

#include <QDebug>
#include <iostream>

namespace puzzle_2018_06 {

Point::Point(int x, int y) : QPoint{x, y} {}

VoronoiDiagram::VoronoiDiagram(const QString& input)
{
  const auto lines = common::splitLines(input);
  m_points.reserve(lines.size());
  for (const auto& line : lines) {
    const auto values = common::toIntValues(line);
    if (values.size() == 2) {
      m_points.emplace_back(values[0], values[1]);
      m_xmin = std::min(m_xmin, values[0]);
      m_ymin = std::min(m_ymin, values[1]);
      m_xmax = std::max(m_xmax, values[0]);
      m_ymax = std::max(m_ymax, values[1]);
    }
  }

  for (auto x = m_xmin; x <= m_xmax; ++x) {
    for (auto y = m_ymin; y <= m_ymax; ++y) {
      const auto closests = closestPoints(x, y);
      if(closests.size() == 1)
        ++m_points[closests[0]].area;
    }
  }

  for (auto x = m_xmin - 1; x <= m_xmax + 1; ++x) {
    auto closests = closestPoints(x, m_ymin);
    for (auto i : closests)
      m_points[i].area = 0;
    closests = closestPoints(x, m_ymax);
    for (auto i : closests)
      m_points[i].area = 0;
  }

  for (auto y = m_ymin - 1; y <= m_ymax + 1; ++y) {
    auto closests = closestPoints(m_xmin, y);
    for (auto i : closests)
      m_points[i].area = 0;
    closests = closestPoints(m_xmax, y);
    for (auto i : closests)
      m_points[i].area = 0;
  }
}

const std::vector<Point>& VoronoiDiagram::points() const { return m_points; }

int VoronoiDiagram::xMin() const { return m_xmin; }

int VoronoiDiagram::yMin() const { return m_ymin; }

int VoronoiDiagram::xMax() const { return m_xmax; }

int VoronoiDiagram::yMax() const { return m_ymax; }

uint VoronoiDiagram::largestArea() const
{
  auto largest = 0u;
  for (const auto& pt : m_points)
    largest = std::max(largest, pt.area);
  return largest;
}

uint VoronoiDiagram::safeRegionArea() const
{
  auto area = 0u;
  for (auto x = m_xmin; x <= m_xmax; ++x) {
    for (auto y = m_ymin; y <= m_ymax; ++y) {
      const auto p = QPoint{x, y};
      auto sum = 0;
      for (const auto& pt : m_points)
        sum += (pt - p).manhattanLength();
      if (sum < 10000)
        ++area;
    }
  }
  return area;
}

std::vector<std::size_t> VoronoiDiagram::closestPoints(int x, int y) const {
  auto min_dist = std::numeric_limits<int>::max();
  auto distances = std::vector<int>{};
  distances.reserve(m_points.size());
  for (const auto& pt : m_points) {
    distances.push_back((QPoint{x, y} - pt).manhattanLength());
    min_dist = std::min(min_dist, distances.back());
  }
  auto res = std::vector<std::size_t>{};
  res.reserve(distances.size());
  for (auto i = std::size_t{0}; i < distances.size(); ++i)
    if (distances[i] == min_dist)
      res.push_back(i);
  return res;
}

}

void Solver_2018_06_1::solve(const QString& input)
{
  const auto diagram = puzzle_2018_06::VoronoiDiagram{input};
  auto* display = new Puzzle_2018_06_Display{diagram};
  display->show();
  emit finished(QString{"%1"}.arg(diagram.largestArea()));
}

void Solver_2018_06_2::solve(const QString& input)
{
  const auto diagram = puzzle_2018_06::VoronoiDiagram{input};
  emit finished(QString{"%1"}.arg(diagram.safeRegionArea()));
}
