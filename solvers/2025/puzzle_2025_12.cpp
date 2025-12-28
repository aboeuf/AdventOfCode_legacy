#include <solvers/2025/puzzle_2025_12.h>
#include <solvers/common.h>

#include <deque>
#include <unordered_set>

#include <boost/functional/hash.hpp>

#include <iostream>

namespace puzzle_2025_12 {

constexpr auto shapes_side_length = 3u;

class Shape {
public:
  Shape(const QStringList &lines) : m_area{0} {
    if (lines.size() != shapes_side_length) {
      throw std::invalid_argument(
          QString("bad number of rows in shape: expected %1 but got %2")
              .arg(shapes_side_length)
              .arg(lines.size())
              .toStdString());
    }
    for (auto i = 0u; i < shapes_side_length; ++i) {
      const auto &line = lines.at(i);
      if (line.size() != shapes_side_length) {
        throw std::invalid_argument(
            QString("bad number of columns in shape: expected %1 but got %2")
                .arg(shapes_side_length)
                .arg(line.size())
                .toStdString());
      }
      for (auto j = 0u; j < shapes_side_length; ++j) {
        if (line.at(j) == "#") {
          ++m_area;
        } else if (line.at(j) != ".") {
          throw std::invalid_argument(QString("invalid shape character '%1'")
                                          .arg(line.at(j))
                                          .toStdString());
        }
      }
    }
  }

  uint area() const { return m_area; }

private:
  uint m_area;
};

class Region {
public:
  Region(QRegExp &rx, const std::vector<Shape> &shapes) {
    auto ok = true;
    const auto length = rx.cap(1).toUInt(&ok);
    if (not ok) {
      throw std::invalid_argument(
          QString("cannot convert string \"%1\" to unsigned integer")
              .arg(rx.cap(1))
              .toStdString());
    }
    const auto width = rx.cap(2).toUInt(&ok);
    if (not ok) {
      throw std::invalid_argument(
          QString("cannot convert string \"%1\" to unsigned integer")
              .arg(rx.cap(2))
              .toStdString());
    }
    const auto presents = common::toVecUInt(rx.cap(3), ' ');
    if (static_cast<std::size_t>(presents.size()) != shapes.size()) {
      throw std::invalid_argument(
          QString("line \"%1\":\nregion parsing nb shapes mismatch: got "
                  "%2 but expected %3")
              .arg(rx.cap(0))
              .arg(presents.size())
              .arg(shapes.size())
              .toStdString());
    }
    const auto nb_available_tiles =
        (length / shapes_side_length) * (width / shapes_side_length);
    auto nb_presents = 0u;
    for (const auto n : presents) {
      nb_presents += n;
    }
    if (nb_presents <= nb_available_tiles) {
      m_valid = true;
      return;
    }
    auto presents_area = 0u;
    for (auto i = 0; i < presents.size(); ++i) {
      presents_area += presents[i] * shapes[i].area();
    }
    if (presents_area > length * width) {
      m_valid = false;
    }
  }

  const std::optional<bool> &valid() const { return m_valid; }

private:
  std::optional<bool> m_valid;
};

class Problem {
public:
  Problem(const QString &input) {
    const auto lines = common::splitLines(input, true);
    auto nb_shapes = 0u;
    auto nb_regions = 0u;
    for (const auto &line : lines) {
      if (line.contains(':')) {
        if (line.contains('x')) {
          ++nb_regions;
        } else {
          ++nb_shapes;
        }
      }
    }
    m_shapes.reserve(nb_regions);
    m_regions.reserve(nb_regions);
    auto current_shape_index = -1;
    auto current_shape = QStringList();
    auto ok = true;
    auto index_rx = QRegExp{"^(\\d+):$"};
    auto region_rx = QRegExp{"^(\\d+)x(\\d+): (.+)$"};
    for (const auto &line : lines) {
      if (index_rx.exactMatch(line)) {
        current_shape_index = index_rx.cap(1).toUInt(&ok);
        if (not ok) {
          throw std::invalid_argument(
              QString("line \"%1\":\ncannot convert string \"%2\" to unsigned "
                      "integer")
                  .arg(line)
                  .arg(index_rx.cap(1))
                  .toStdString());
        }
        if (static_cast<std::size_t>(current_shape_index) != m_shapes.size()) {
          throw std::invalid_argument(
              QString("line \"%1\":\nindex mismatch: got %1 but expected %2")
                  .arg(line)
                  .arg(current_shape_index)
                  .arg(m_shapes.size())
                  .toStdString());
        }
        current_shape.clear();
      } else if (region_rx.exactMatch(line)) {
        m_regions.emplace_back(region_rx, m_shapes);
      } else {
        current_shape << line;
      }
      if (static_cast<unsigned int>(current_shape.size()) ==
          shapes_side_length) {
        m_shapes.emplace_back(current_shape);
        current_shape.clear();
      }
    }
  }

  QString solve() const {
    auto nb_valid = 0u;
    for (const auto &region : m_regions) {
      const auto &valid = region.valid();
      if (not valid.has_value()) {
        return "Bad input";
      }
      if (*valid) {
        ++nb_valid;
      }
    }
    return QString("%1").arg(nb_valid);
  }

private:
  std::vector<Shape> m_shapes;
  std::vector<Region> m_regions;
};

} // namespace puzzle_2025_12

void Solver_2025_12_1::solve(const QString &input) {
  const auto pb = puzzle_2025_12::Problem(input);
  emit finished(pb.solve());
}

void Solver_2025_12_2::solve(const QString &input) {
  emit finished("Merry Christmas!");
}
