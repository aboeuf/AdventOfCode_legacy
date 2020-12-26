#pragma once
#include <solvers.h>
#include <set>

namespace puzzle_2020_24
{

struct Tile
{
  Tile() = default;
  Tile(int x, int y);

  bool operator < (const Tile& other) const;
  Tile operator + (const Tile& other) const;
  void operator += (const Tile& other);

  int m_x{0};
  int m_y{0};
};

class Lobby : public QObject
{
  Q_OBJECT

public:
  Lobby(const QString& input);

  bool isBlackTile(const Tile& t) const;
  uint nbBlackNeighbors(const Tile& t, std::set<Tile>* white_neighbors = nullptr) const;
  QString toString() const;

  void flipTile(const QString& coordinates);

  std::set<Tile> m_tiles;

signals:
  void finished();

public slots:
  void advanceDays(uint nb_days);
};

}

class Solver_2020_24_1 : public Solver
{
public:
  void solve(const QString& input) override;
};

class Solver_2020_24_2 : public Solver
{
public:
  void solve(const QString& input) override;
};
