#include <2020/puzzle_2020_3.h>
#include <common.h>
#include <QDebug>

namespace puzzle_2020_3 {

using Map = std::vector<std::vector<bool>>;

Map getMap(const QString& input)
{
  Map map;
  QStringList lines = common::splitLines(input);
  map.resize(lines.size());
  std::size_t i = 0u;
  for (const QString& line : lines) {
    map[i].reserve(line.size());
    for (const QString& c : line) {
      map[i].emplace_back(c == '#');
    }
    ++i;
  }
  return map;
}

unsigned int checkSlope(const Map& map, std::size_t right, std::size_t down)
{
  unsigned int nb_trees = 0u;
  std::size_t column = 0u;
  for (std::size_t line = 0; line < map.size(); line += down) {
    if (column >= map[line].size())
      column -= map[line].size();
    if (map[line][column])
      ++nb_trees;
    column += right;
  }
  return nb_trees;
}

}

void Solver_2020_3_1::solve(const QString& input) const
{
  using namespace puzzle_2020_3;
  const Map map = puzzle_2020_3::getMap(input);
  emit finished(QString::number(checkSlope(map, 3, 1)));
}

void Solver_2020_3_2::solve(const QString& input) const
{
  using namespace puzzle_2020_3;
  const Map map = puzzle_2020_3::getMap(input);
  emit finished(QString::number(checkSlope(map, 1, 1) *
                                checkSlope(map, 3, 1) *
                                checkSlope(map, 5, 1) *
                                checkSlope(map, 7, 1) *
                                checkSlope(map, 1, 2)));
}

