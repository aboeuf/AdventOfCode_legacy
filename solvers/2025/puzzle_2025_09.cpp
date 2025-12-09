#include <solvers/2025/puzzle_2025_09.h>
#include <solvers/common.h>

namespace puzzle_2025_09 {

using Int = unsigned long long int;

struct Tile {
  Tile(Int x, Int y) : x{x}, y{y} {}

  Tile(const QString &input) {
    const auto values = common::toVecULongLong(input);
    if (values.size() != 2) {
      std::invalid_argument("cannot parse tile input");
    }
    x = values.front();
    y = values.back();
  }

  QPointF toPointF() const {
    return QPointF(1.e-3 * static_cast<qreal>(x),
                   1.e-3 * static_cast<qreal>(y));
  }

  Int x;
  Int y;
};

class Rectangle {
public:
  Rectangle(const Tile &first_corner, const Tile &second_corner)
      : m_min_corner{std::min(first_corner.x, second_corner.x),
                     std::min(first_corner.y, second_corner.y)},
        m_max_corner{std::max(first_corner.x, second_corner.x),
                     std::max(first_corner.y, second_corner.y)} {}

  const Tile &minCorner() const { return m_min_corner; }
  const Tile &maxCorner() const { return m_max_corner; }

  Int area() const {
    return (m_max_corner.x - m_min_corner.x + 1) *
           (m_max_corner.y - m_min_corner.y + 1);
  }

  bool contains(const Tile &tile) const {
    return m_min_corner.x < tile.x and tile.x < m_max_corner.x and
           m_min_corner.y < tile.y and tile.y < m_max_corner.y;
  }

  QRectF toRecF() const {
    return QRectF(1.e-3 * static_cast<qreal>(minCorner().x),
                  1.e-3 * static_cast<qreal>(minCorner().y),
                  1.e-3 * static_cast<qreal>(m_max_corner.x - m_min_corner.x),
                  1.e-3 * static_cast<qreal>(m_max_corner.y - m_min_corner.y));
  }

private:
  Tile m_min_corner;
  Tile m_max_corner;
};

inline bool segMapContains(const std::map<Int, std::map<Int, Int>> &seg_map,
                           const Int &first_key, const Int &second_key) {
  const auto first_it = seg_map.find(first_key);
  if (first_it == std::cend(seg_map)) {
    return false;
  }
  auto second_it = first_it->second.upper_bound(second_key);
  if (second_it == std::cbegin(first_it->second)) {
    return false;
  }
  --second_it;
  if (second_it == std::cend(first_it->second)) {
    return false;
  }
  return second_key <= second_it->second;
}

class Tiles {
public:
  Tiles(const QString &input) {
    const auto lines = common::splitLines(input);
    m_tiles.reserve(lines.size());
    for (const auto &line : lines) {
      m_tiles.emplace_back(line);
    }
    if (m_tiles.empty()) {
      return;
    }
    m_rectangles.reserve(m_tiles.size() * (m_tiles.size() - 1u));
    for (auto i = 0u; i < m_tiles.size(); ++i) {
      const auto next = i + 1 < m_tiles.size() ? i + 1u : 0u;
      if (m_tiles[i].x == m_tiles[next].x) {
        m_vertical_segments[m_tiles[i].x]
                           [std::min(m_tiles[i].y, m_tiles[next].y)] =
                               std::max(m_tiles[i].y, m_tiles[next].y);
      } else if (m_tiles[i].y == m_tiles[next].y) {
        m_horizontal_segments[m_tiles[i].y]
                             [std::min(m_tiles[i].x, m_tiles[next].x)] =
                                 std::max(m_tiles[i].x, m_tiles[next].x);
      } else {
        throw std::invalid_argument("segment is not axis-aligned");
      }
      for (auto j = i + 1u; j < m_tiles.size(); ++j) {
        m_rectangles.emplace_back(m_tiles[i], m_tiles[j]);
      }
    }
    std::sort(std::begin(m_rectangles), std::end(m_rectangles),
              [](const Rectangle &lhs, const Rectangle &rhs) {
                return lhs.area() > rhs.area();
              });
    auto polygon = QPolygonF();
    for (const auto &tile : m_tiles) {
      polygon << tile.toPointF();
    }
    m_display_data.polygons.emplace_back(std::move(polygon),
                                         QPen(QBrush(QColor("black")), 0.1,
                                              Qt::SolidLine, Qt::RoundCap,
                                              Qt::RoundJoin),
                                         QBrush());
  }

  const DisplayData &displayData() const { return m_display_data; }

  QString solveOne() const {
    if (m_rectangles.empty()) {
      return "empty";
    }
    m_display_data.rectangles.emplace_back(m_rectangles.front().toRecF(),
                                           QPen(QBrush(QColor("red")), 0.1,
                                                Qt::SolidLine, Qt::RoundCap,
                                                Qt::RoundJoin),
                                           QBrush());
    return QString("%1").arg(m_rectangles.front().area());
  }

  QString solveTwo() const {
    for (const auto &rectangle : m_rectangles) {
      if (containsRectangle(rectangle)) {
        m_display_data.rectangles.emplace_back(rectangle.toRecF(),
                                               QPen(QBrush(QColor("red")), 0.1,
                                                    Qt::SolidLine, Qt::RoundCap,
                                                    Qt::RoundJoin),
                                               QBrush());
        return QString("%1").arg(rectangle.area());
      }
    }
    return "failure";
  }

private:
  bool containsTile(Int x, Int y) const {
    const auto x_it = m_tested_tiles.find(x);
    if (x_it != m_tested_tiles.cend()) {
      const auto y_it = x_it->second.find(y);
      if (y_it != x_it->second.cend()) {
        return y_it->second;
      }
    }
    if (segMapContains(m_horizontal_segments, y, x) or
        segMapContains(m_vertical_segments, x, y)) {
      m_tested_tiles[x][y] = true;
      return true;
    }
    auto it = m_vertical_segments.upper_bound(x);
    if (it == std::cbegin(m_vertical_segments)) {
      m_tested_tiles[x][y] = false;
      return false;
    }
    auto nb_intersections = 0u;
    for (; it != std::cend(m_vertical_segments); ++it) {
      for (const auto &[y_start, y_end] : it->second) {
        if (y_start <= y and y <= y_end) {
          ++nb_intersections;
        }
      }
    }
    const auto is_inside = nb_intersections % 2u == 1u;
    m_tested_tiles[x][y] = is_inside;
    return is_inside;
  }

  bool intersectsSegment(const Tile &from, const Tile &to) const {
    if (from.x != to.x and from.y != to.y) {
      throw std::invalid_argument("segment is not axis-aligned");
    }
    const auto &segments =
        from.x == to.x ? m_horizontal_segments : m_vertical_segments;
    const auto constant = from.x == to.x ? from.x : from.y;
    const auto begin =
        from.x == to.x ? std::min(from.y, to.y) : std::min(from.x, to.x);
    const auto end =
        from.x == to.x ? std::max(from.y, to.y) : std::max(from.x, to.x);
    for (auto it = segments.upper_bound(begin);
         it != segments.cend() and it->first < end; ++it) {
      auto seg = it->second.upper_bound(constant);
      if (seg == it->second.cbegin()) {
        continue;
      }
      --seg;
      if (seg == it->second.cend()) {
        continue;
      }
      if (seg->first < constant and constant < seg->second) {
          return true;
      }
    }
    return false;
  }

  bool containsRectangle(const Rectangle &rectangle) const {
    const auto corners = std::array<Tile, 4>{
        rectangle.minCorner(),
        Tile(rectangle.maxCorner().x, rectangle.minCorner().y),
        rectangle.maxCorner(),
        Tile(rectangle.minCorner().x, rectangle.maxCorner().y)};
    if (std::any_of(std::cbegin(corners), std::cend(corners),
                    [this](const Tile &corner) {
                      return not containsTile(corner.x, corner.y);
                    })) {
      return false;
    }
    if (std::any_of(std::cbegin(m_tiles), std::cend(m_tiles),
                    [this, &rectangle](const Tile &tile) {
                      return rectangle.contains(tile);
                    })) {
      return false;
    }
    for (auto i = 0u; i < corners.size(); ++i) {
      const auto next = i + 1 < corners.size() ? i + 1u : 0u;
      if (intersectsSegment(corners[i], corners[next])) {
        return false;
      }
    }
    return true;
  }

  std::vector<Tile> m_tiles;
  std::vector<Rectangle> m_rectangles;
  std::map<Int, std::map<Int, Int>> m_horizontal_segments;
  std::map<Int, std::map<Int, Int>> m_vertical_segments;
  mutable std::unordered_map<Int, std::unordered_map<Int, bool>> m_tested_tiles;
  mutable DisplayData m_display_data;
};

} // namespace puzzle_2025_09

void Solver_2025_09_1::solve(const QString &input) {
  const auto tiles = puzzle_2025_09::Tiles(input);
  const auto solution = tiles.solveOne();
  emit display(tiles.displayData());
  emit finished(solution);
}

void Solver_2025_09_2::solve(const QString &input) {
  const auto tiles = puzzle_2025_09::Tiles(input);
  const auto solution = tiles.solveTwo();
  emit display(tiles.displayData());
  emit finished(solution);
}
