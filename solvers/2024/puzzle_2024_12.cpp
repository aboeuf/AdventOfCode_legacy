#include <solvers/common.h>
#include <solvers/2024/puzzle_2024_12.h>

#include <solvers/qpoint_hash.hpp>

namespace puzzle_2024_12 {

const auto cardinal_directions =
    QSet<QPoint>{QPoint{-1, 0}, QPoint{1, 0}, QPoint{0, -1}, QPoint{0, 1}};

class GardenPlots {
public:
  GardenPlots(const QString &input) {
    const auto lines = common::splitLines(input);
    m_nb_rows = lines.size();
    if (m_nb_rows == 0) {
      common::throwInvalidArgumentError(
          "puzzle_2024_12::GardenPlots: empty input");
    }
    m_garden_plots.reserve(m_nb_rows);
    m_nb_columns = lines.front().size();
    for (const auto &line : lines) {
      if (line.size() != m_nb_columns) {
        common::throwInvalidArgumentError(
            "puzzle_2024_12::GardenPlots: inconsistent number of columns");
      }
      m_garden_plots.emplace_back();
      m_garden_plots.back().reserve(m_nb_columns);
      for (const auto &plant_type : line) {
        m_garden_plots.back().emplace_back(plant_type);
      }
    }
  }

  int nbRows() const { return m_nb_rows; }
  int nbColumns() const { return m_nb_columns; }

  bool contains(const QPoint &position) const {
    return position.x() >= 0 and position.x() < m_nb_rows and
           position.y() >= 0 and position.y() < m_nb_columns;
  }

  const QChar &plantTypeAt(const QPoint &position) const {
    if (not contains(position)) {
      common::throwInvalidArgumentError(
          QString("puzzle_2024_12::GardenPlots::plantTypeAt: out ouf range "
                  "position (%1, %2): size is (%3, %4)")
              .arg(position.x())
              .arg(position.y())
              .arg(m_nb_rows)
              .arg(m_nb_columns));
    }
    return m_garden_plots[position.x()][position.y()];
  }

private:
  int m_nb_rows;
  int m_nb_columns;
  std::vector<std::vector<QChar>> m_garden_plots;
};

class Region {
public:
  Region(const QPoint &position, const GardenPlots &plots,
         QSet<QPoint> &treated_plots) {
    m_plant_type = plots.plantTypeAt(position);
    addPosition(position, plots, treated_plots);
    m_area = m_garden_plots.size();
    auto sides = QHash<QString, QList<int>>{};
    for (const auto &position : m_garden_plots) {
      for (const auto direction : cardinal_directions) {
        const auto neighbor = position + direction;
        if (not m_garden_plots.contains(neighbor)) {
          ++m_perimeter;
          if (direction == QPoint(-1, 0)) {
            sides[QString("N%1").arg(position.x())] << position.y();
          } else if (direction == QPoint(1, 0)) {
            sides[QString("S%1").arg(position.x())] << position.y();
          } else if (direction == QPoint(0, -1)) {
            sides[QString("E%1").arg(position.y())] << position.x();
          } else {
            sides[QString("W%1").arg(position.y())] << position.x();
          }
        }
      }
    }
    for (auto hash_it = std::begin(sides); hash_it != std::end(sides);
         ++hash_it) {
      auto &list = hash_it.value();
      std::sort(std::begin(list), std::end(list));
      for (auto list_it = std::begin(list);
           std::next(list_it) != std::end(list);) {
        if (*std::next(list_it) == *list_it + 1) {
          list_it = list.erase(list_it);
        } else {
          ++list_it;
        }
      }
      m_nb_sides += list.size();
    }
  }

  int price(bool bulk_discount) const {
    return m_area * (bulk_discount ? m_nb_sides : m_perimeter);
  }

private:
  void addPosition(const QPoint &position, const GardenPlots &plots,
                   QSet<QPoint> &treated_plots) {
    if (plots.contains(position) and not treated_plots.contains(position) and
        plots.plantTypeAt(position) == m_plant_type) {
      treated_plots.insert(position);
      m_garden_plots.insert(position);
      for (const auto &direction : cardinal_directions) {
        addPosition(position + direction, plots, treated_plots);
      }
    }
  }

  QChar m_plant_type;
  QSet<QPoint> m_garden_plots;
  int m_area{0};
  int m_perimeter{0};
  int m_nb_sides{0};
};

class Regions {
public:
  Regions(const QString &input) : m_garden_plots{input} {
    auto treated_plots = QSet<QPoint>{};
    auto current_position = QPoint(0, 0);
    for (; current_position.x() < m_garden_plots.nbRows();
         ++current_position.rx()) {
      current_position.setY(0);
      for (; current_position.y() < m_garden_plots.nbColumns();
           ++current_position.ry()) {
        if (not treated_plots.contains(current_position)) {
          m_regions << Region(current_position, m_garden_plots, treated_plots);
        }
      }
    }
  }

  int price(bool bulk_discount) const {
    auto price = 0;
    for (const auto &region : m_regions) {
      price += region.price(bulk_discount);
    }
    return price;
  }

private:
  GardenPlots m_garden_plots;
  QList<Region> m_regions;
};

} // namespace puzzle_2024_12

void Solver_2024_12_1::solve(const QString &input) {
  emit finished(QString("%1").arg(puzzle_2024_12::Regions(input).price(false)));
}

void Solver_2024_12_2::solve(const QString &input) {
  emit finished(QString("%1").arg(puzzle_2024_12::Regions(input).price(true)));
}
