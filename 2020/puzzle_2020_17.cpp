#include <2020/puzzle_2020_17.h>
#include <common.h>
#include <QVector>
#include <map>
#include <set>
#include <QDebug>

namespace puzzle_2020_17
{

struct GridPoint : QVector<int>
{
  bool operator < (const GridPoint& other) const
  {
    if (this->size() != other.size())
      return this->size() < other.size();
    for (int i = 0; i < this->size(); ++i) {
      if (this->at(i) < other[i])
        return true;
      if (this->at(i) > other[i])
        return false;
    }
    return false;
  }

  GridPoint operator + (const GridPoint& other) const
  {
    int min_size = std::min(this->size(), other.size());
    GridPoint sum{};
    sum.resize(min_size);
    for (int i = 0; i < min_size; ++i)
      sum[i] = this->at(i) + other.at(i);
    return sum;
  }
};

struct Cube
{
  Cube(bool active = false) : m_is_active{active} {}

  uint nbActiveNeighbors() const
  {
    uint nb_active{0};
    for (Cube* neighbor : m_neighbors)
      if (neighbor && neighbor->m_is_active)
        ++nb_active;
    return nb_active;
  }

  void computeNextState(bool puzzle_1)
  {
    m_is_changing_next = false;
    uint nb_active_neighbors = nbActiveNeighbors();
    if (puzzle_1) {
      if (m_is_active) {
        if (nb_active_neighbors != 2 && nb_active_neighbors != 3)
          m_is_changing_next = true;
      } else if (nb_active_neighbors == 3)
        m_is_changing_next = true;
    }
  }

  std::set<Cube*> m_neighbors{};
  bool m_is_active{false};
  bool m_is_changing_next{false};
};

class Grid
{
public:
  Grid(const QString& input, int dimension)
  {
    m_dimension = std::max(dimension, 2);
    m_min.fill(0, m_dimension);
    m_max.fill(0, m_dimension);
    GridPoint pt;
    pt.fill(0, m_dimension);
    QList<GridPoint> points;
    const QStringList& lines = common::splitLines(input);
    pt[0] = 0;
    for (const QString& line : lines) {
      pt[1] = 0;
      for (const QChar& c : line) {
        addCube(pt, c == '#');
        points << pt;
        if (c == '#')
          ++m_nb_active;
        ++pt[0];
      }
      ++pt[1];
    }
    for (const GridPoint& pt : points)
      linkCube(pt);
  }

//  void oneStep(bool puzzle_1)
//  {
//    for (int i = 0; i < m_dimension; ++i) {
//      m_min[i] -= 1;
//      m_max[i] += 1;
//    }
//    std::set<GridPoint> new_cubes;
//    GridPoint pt;
//    pt.resize(m_dimension);
//    int binary_size = QString::number(m_dimension, 0, 2).size();
//    for (int i = 0; i < m_dimension; ++i) {
//      QString binary = QString::number(i, binary_size,  pt[i] = m_min[i];
//      for (int j = 0; j < m_dimension; ++j) {
//    }


//    for (int x = x_min; x <= x_max; ++x) {
//      for (int y = y_min; y <= y_max; ++y) {
//        for (int z = z_min; z <= z_max; ++z) {
//          for (int w = w_min; w <= w_max; ++w) {
//            if (x == x_min || x == x_max ||
//                y == y_min || y == y_max ||
//                z == z_min || z == z_max ||
//                w == w_min || w == w_max) {
//              new_cubes << GridPoint(x, y, z, w);
//              addCube(new_cubes.back());
//            }
//          }
//        }
//      }
//    }
//    for (const GridPoint& pt : new_cubes)
//      linkCube(pt);
//    computeNext(puzzle_1);
//    update();
//  }

//  uint nbActive() const
//  {
//    return m_nb_active;
//  }

private:
  Cube* at(const GridPoint& pt)
  {
    auto it = m_cubes.find(pt);
    if (it == m_cubes.end())
      return nullptr;
    return &it->second;
  }

    void addCube(const GridPoint& pt, bool active = false)
    {
      if (pt.size() != m_dimension || at(pt) != nullptr)
        return;
      for (int i = 0; i < m_dimension; ++i) {
        m_min[i] = std::min(m_min[i], pt[i]);
        m_max[i] = std::max(m_max[i], pt[i]);
      }
      m_cubes[pt] = Cube{active};
    }

  void linkCube(const GridPoint& pt)
  {
    if (pt.size() != m_dimension)
      return;
//    QList<GridPoint> neighbors;
//    for (int i = 0; i < m_dimension; ++i) {
//      QList<GridPoint> tmp = neighbors;
//    pt

//    Cube* cube = at(pt);
//    if (!cube)
//      return;
//    GridPoint pt;
//    pt.fill(0, m_dimension);

//    for (int i = -1; i < 2; ++i) {
//      for (int j = -1; j < 2; ++j) {
//        for (int k = -1; k < 2; ++k) {
//          for (int l = -1; l < 2; ++l) {
//            Cube* neighbor = at(pt + GridPoint(i, j, k, l));
//            if (neighbor && neighbor != cube) {
//              cube->m_neighbors.insert(neighbor);
//              neighbor->m_neighbors.insert(cube);
//            }
//          }
//        }
//      }
//    }
  }



//  void computeNext(bool puzzle_1)
//  {
//    for (auto it = m_cubes.begin(); it != m_cubes.end(); ++it)
//      it->second.computeNextState(puzzle_1);
//  }

//  void update()
//  {
//    m_nb_active = 0;
//    for (auto it = m_cubes.begin(); it != m_cubes.end(); ++it) {
//      Cube* cube = &(it->second);
//      if (cube->m_is_changing_next)
//        cube->m_is_active = !cube->m_is_active;
//      if (cube->m_is_active)
//        ++m_nb_active;
//    }
//  }

  std::map<GridPoint, Cube> m_cubes{};
  uint m_nb_active{0};
  int m_dimension{0};
  GridPoint m_min{};
  GridPoint m_max{};
};

} // namespace puzzle_2020_17

void Solver_2020_17_1::solve(const QString& input)
{
//  using namespace puzzle_2020_17;
//  Grid grid(input);
//  for (int i = 0; i < 6; ++i) {
//    grid.oneStep(true);
//  }
//  emit finished(QString::number(grid.nbActive()));
  emit finished(input);
}

void Solver_2020_17_2::solve(const QString& input)
{
  int binary_size = QString("%1").arg(10, 0, 3).size();
  for (int i = 0; i < 8; ++i)
    qDebug() << QString("%1").arg(i, binary_size, 3, QChar('0'));
  emit finished(input);
}

