#include <solvers/2020/display/puzzle_2020_24/puzzle_2020_24_display.h>
#include <solvers/2020/puzzle_2020_24.h>
#include <solvers/common.h>

namespace puzzle_2020_24 {

Tile::Tile(int x, int y) : m_x{x}, m_y{y} {}

bool Tile::operator<(const Tile &other) const {
  if (m_y == other.m_y)
    return m_x < other.m_x;
  return m_y < other.m_y;
}

Tile Tile::operator+(const Tile &other) const {
  return Tile{m_x + other.m_x, m_y + other.m_y};
}

void Tile::operator+=(const Tile &other) {
  m_x += other.m_x;
  m_y += other.m_y;
}

const auto directions =
    QMap<QString, Tile>{{"e", {1, 0}},  {"ne", {0, 1}},  {"nw", {-1, 1}},
                        {"w", {-1, 0}}, {"sw", {0, -1}}, {"se", {1, -1}}};

const auto default_direction = Tile{0, 0};

Lobby::Lobby(const QString &input) {
  const QStringList lines = common::splitLines(input);
  for (const QString line : lines)
    flipTile(line);
}

bool Lobby::isBlackTile(const Tile &t) const {
  return m_tiles.find(t) != m_tiles.end();
}

uint Lobby::nbBlackNeighbors(const Tile &t,
                             std::set<Tile> *white_neighbors) const {
  uint sum = 0;
  for (const Tile &d : directions.values()) {
    const Tile n = t + d;
    if (isBlackTile(n))
      ++sum;
    else if (white_neighbors)
      white_neighbors->insert(n);
  }
  return sum;
}

QString Lobby::toString() const {
  QString s;
  for (const Tile &t : m_tiles)
    s += QString("(%1, %2)\n").arg(t.m_x).arg(t.m_y);
  return s;
}

void Lobby::flipTile(const QString &coordinates) {
  Tile tile{0, 0};
  for (auto c = coordinates.begin(); c != coordinates.end(); ++c) {
    QString d{*c};
    if (*c != 'e' && *c != 'w') {
      ++c;
      if (c != coordinates.end())
        d.push_back(*c);
    }
    tile += directions.value(d, default_direction);
  }
  auto tile_it = m_tiles.find(tile);
  if (tile_it == m_tiles.end())
    m_tiles.insert(tile);
  else
    m_tiles.erase(tile_it);
}

void Lobby::advanceDays(uint nb_days) {
  for (uint i = 0; i < nb_days; ++i) {
    QList<Tile> black_next, white_next;
    std::set<Tile> white_neighbors;
    for (const Tile &t : m_tiles) {
      uint nb_black_neighbors = nbBlackNeighbors(t, &white_neighbors);
      if (nb_black_neighbors == 0 || nb_black_neighbors > 2)
        white_next << t;
    }
    for (const Tile &t : white_neighbors)
      if (nbBlackNeighbors(t) == 2)
        black_next << t;
    for (const Tile &t : white_next)
      m_tiles.erase(t);
    for (const Tile &t : black_next)
      m_tiles.insert(t);
  }
  emit finished();
}

} // namespace puzzle_2020_24

void Solver_2020_24_1::solve(const QString &input) {
  using namespace puzzle_2020_24;
  Lobby *lobby = new Lobby(input);
  Puzzle_2020_24_Display *display = new Puzzle_2020_24_Display(lobby);
  display->show();
  emit finished(QString::number(lobby->m_tiles.size()));
}

void Solver_2020_24_2::solve(const QString &input) {
  using namespace puzzle_2020_24;
  Lobby loby(input);
  loby.advanceDays(100);
  emit finished(QString::number(loby.m_tiles.size()));
}
