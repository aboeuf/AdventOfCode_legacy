#include <solvers/common.h>
#include <solvers/qpoint_hash.hpp>
#include <solvers/2024/puzzle_2024_08.h>

#include <QHash>

namespace puzzle_2024_08 {

class Antennas {
public:
  Antennas(const QString &input) {
    auto lines = common::splitLines(input);
    m_nb_rows = lines.size();
    if (m_nb_rows == 0) {
      throw std::invalid_argument("puzzle_2024_06::Map: empty map");
    }
    m_nb_columns = lines.front().size();
    auto row = 0;
    for (const auto &line : lines) {
      if (line.size() != m_nb_columns) {
        throw std::invalid_argument(
            QString("puzzle_2024_08::Antennas: incoherent number of columns at "
                    "line "
                    "%1 (expected %2 but got %3")
                .arg(row + 1u)
                .arg(m_nb_columns)
                .arg(line.size())
                .toStdString());
      }
      auto column = 0;
      for (const auto &c : line) {
        if (c != '.') {
          m_antennas[c].insert(QPoint(row, column));
        }
        ++column;
      }
      ++row;
    }
  }

  QString solveOne() const {
    auto antinodes = QSet<QPoint>{};
    for (auto it = m_antennas.cbegin(); it != m_antennas.cend(); ++it) {
      for (auto first = it.value().cbegin(); first != it.value().cend();
           ++first) {
        for (auto second = std::next(first); second != it.value().cend();
             ++second) {
          const auto delta = *second - *first;
          const auto antennas_antinodes =
              QVector<QPoint>{*first - delta, *second + delta};
          for (const auto &antinode : antennas_antinodes) {
            if (antinode.x() >= 0 and antinode.x() < m_nb_rows and
                antinode.y() >= 0 and antinode.y() < m_nb_columns) {
              antinodes.insert(antinode);
            }
          }
        }
      }
    }
    return QString("%1").arg(antinodes.size());
  }

  QString solveTwo() const {
    auto antinodes = QSet<QPoint>{};
    for (auto it = m_antennas.cbegin(); it != m_antennas.cend(); ++it) {
      for (auto first = it.value().cbegin(); first != it.value().cend();
           ++first) {
        for (auto second = std::next(first); second != it.value().cend();
             ++second) {
          const auto deltas =
              QVector<QPoint>{*first - *second, *second - *first};
          for (const auto &delta : deltas) {
            auto antinode = *first;
            while (antinode.x() >= 0 and antinode.x() < m_nb_rows and
                   antinode.y() >= 0 and antinode.y() < m_nb_columns) {
              antinodes.insert(antinode);
              antinode += delta;
            }
          }
        }
      }
    }
    return QString("%1").arg(antinodes.size());
  }

private:
  int m_nb_rows;
  int m_nb_columns;
  QHash<QChar, QSet<QPoint>> m_antennas;
};

} // namespace puzzle_2024_08

void Solver_2024_08_1::solve(const QString &input) {
  const auto antennas = puzzle_2024_08::Antennas{input};
  emit finished(antennas.solveOne());
}

void Solver_2024_08_2::solve(const QString &input) {
  const auto antennas = puzzle_2024_08::Antennas{input};
  emit finished(antennas.solveTwo());
}
