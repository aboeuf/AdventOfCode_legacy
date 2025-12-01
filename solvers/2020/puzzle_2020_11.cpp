#include <QDebug>
#include <QVector>
#include <solvers/2020/puzzle_2020_11.h>
#include <solvers/common.h>

namespace puzzle_2020_11 {

struct Cell {
  Cell() = default;
  virtual ~Cell() = default;

  virtual QChar getChar() const = 0;
  virtual bool isSeat() const = 0;
  virtual bool isEmptySeat() const = 0;
  virtual bool isOccupiedSeat() const = 0;
  virtual void computeNextState(bool) = 0;
  virtual bool update() = 0;

  uint nbOccupiedAdjacent() {
    uint nb_occupied = 0;
    for (uint i = 0; i < 8; ++i)
      if (m_neighbors[i] && m_neighbors[i]->isOccupiedSeat())
        ++nb_occupied;
    return nb_occupied;
  }

  uint nbOccupiedVisible() {
    uint nb_occupied = 0;
    for (uint i = 0; i < 8; ++i) {
      Cell *current = this;
      do {
        current = current->m_neighbors[i];
      } while (current && !current->isSeat());
      if (current && current->isSeat() && current->isOccupiedSeat())
        ++nb_occupied;
    }
    return nb_occupied;
  }

  QVector<Cell *> m_neighbors{8, nullptr};
};

struct Floor : public Cell {
  Floor() : Cell() {}
  QChar getChar() const override { return '.'; }
  bool isSeat() const override { return false; }
  bool isEmptySeat() const override { return false; }
  bool isOccupiedSeat() const override { return false; }
  void computeNextState(bool) override {}
  bool update() override { return false; }
};

class Seat : public Cell {
public:
  Seat() : Cell() {}

  QChar getChar() const override { return m_is_empty ? 'L' : '#'; }
  bool isSeat() const override { return true; }
  bool isEmptySeat() const override { return m_is_empty; }
  bool isOccupiedSeat() const override { return !m_is_empty; }

  void computeNextState(bool new_rules) override {
    if (new_rules)
      m_is_changing =
          m_is_empty ? nbOccupiedVisible() == 0 : nbOccupiedVisible() > 4;
    else
      m_is_changing =
          m_is_empty ? nbOccupiedAdjacent() == 0 : nbOccupiedAdjacent() > 3;
  }

  bool update() override {
    if (m_is_changing)
      m_is_empty = !m_is_empty;
    return m_is_changing;
  }

private:
  bool m_is_empty{true};
  bool m_is_changing{false};
};

class SeatMap {
public:
  SeatMap(const QString &input) {
    const QStringList lines = common::splitLines(input);
    nb_rows = lines.size();
    nb_columns = 0;
    for (const QString &line : lines)
      nb_columns = std::max(nb_columns, static_cast<uint>(line.size()));
    m_map = QVector<QVector<Cell *>>(nb_columns,
                                     QVector<Cell *>(nb_columns, nullptr));
    uint i = 0;
    for (const QString &line : lines) {
      for (uint j = 0; j < nb_columns; ++j) {
        if (j < static_cast<uint>(line.size()) && line[j] == 'L') {
          m_map[i][j] = new Seat();
          m_empty_seats << m_map[i][j];
        } else {
          m_map[i][j] = new Floor();
          m_floor_cells << m_map[i][j];
        }
      }
      ++i;
    }
    for (i = 0; i < nb_rows; ++i) {
      const bool not_first_row = (i != 0);
      const bool not_last_row = (i + 1 < nb_rows);
      for (uint j = 0; j < nb_columns; ++j) {
        const bool not_first_column = (j != 0);
        const bool not_last_column = (j + 1 < nb_columns);
        QVector<Cell *> &N = m_map[i][j]->m_neighbors;
        if (not_first_row) {
          if (not_first_column)
            N[3] = m_map[i - 1][j - 1];
          if (not_last_column)
            N[1] = m_map[i - 1][j + 1];
          N[2] = m_map[i - 1][j];
        }
        if (not_last_row) {
          if (not_first_column)
            N[5] = m_map[i + 1][j - 1];
          if (not_last_column)
            N[7] = m_map[i + 1][j + 1];
          N[6] = m_map[i + 1][j];
        }
        if (not_first_column)
          N[4] = m_map[i][j - 1];
        if (not_last_column)
          N[0] = m_map[i][j + 1];
      }
    }
  }

  ~SeatMap() {
    for (QVector<Cell *> &row : m_map)
      for (Cell *cell : row)
        delete cell;
  }

  const QList<Cell *> &emptySeats() const { return m_empty_seats; }
  const QList<Cell *> &occupiedSeats() const { return m_occupied_seats; }
  const QList<Cell *> &floorCells() const { return m_floor_cells; }

  Cell *operator()(uint i, uint j) const {
    if (i < nb_rows && j < nb_columns)
      return m_map[i][j];
    return nullptr;
  }

  uint update() {
    uint nb_changes = 0;
    m_empty_seats.clear();
    m_occupied_seats.clear();
    for (uint i = 0; i < nb_rows; ++i) {
      for (uint j = 0; j < nb_columns; ++j) {
        if (m_map[i][j]->update())
          ++nb_changes;
        if (m_map[i][j]->isEmptySeat())
          m_empty_seats << m_map[i][j];
        else if (m_map[i][j]->isOccupiedSeat())
          m_occupied_seats << m_map[i][j];
      }
    }
    return nb_changes;
  }

  void computeNextState(bool new_rules) {
    for (uint i = 0; i < nb_rows; ++i)
      for (uint j = 0; j < nb_columns; ++j)
        m_map[i][j]->computeNextState(new_rules);
  }

  uint applyRound(bool new_rules) {
    computeNextState(new_rules);
    return update();
  }

  void debug(bool new_rule) const {
    qDebug() << "";
    for (uint i = 0; i < nb_columns; ++i) {
      QString str;
      for (uint j = 0; j < nb_columns; ++j)
        str.push_back(m_map[i][j]->getChar());
      str += "  ";
      for (uint j = 0; j < nb_columns; ++j)
        if (new_rule)
          str += QString::number(m_map[i][j]->nbOccupiedVisible());
        else
          str += QString::number(m_map[i][j]->nbOccupiedAdjacent());
      qDebug() << str.toStdString().c_str();
    }
  }

private:
  QVector<QVector<Cell *>> m_map;
  QList<Cell *> m_empty_seats;
  QList<Cell *> m_occupied_seats;
  QList<Cell *> m_floor_cells;
  uint nb_rows{0};
  uint nb_columns{0};
};

} // namespace puzzle_2020_11

void Solver_2020_11_1::solve(const QString &input) {
  using namespace puzzle_2020_11;
  SeatMap map(input);
  while (map.applyRound(false) != 0) {
  }
  emit finished(QString::number(map.occupiedSeats().size()));
}

void Solver_2020_11_2::solve(const QString &input) {
  using namespace puzzle_2020_11;
  SeatMap map(input);
  while (map.applyRound(true) != 0) {
  }
  emit finished(QString::number(map.occupiedSeats().size()));
}
