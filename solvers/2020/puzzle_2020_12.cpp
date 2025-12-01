#include <QPoint>
#include <QVector>
#include <solvers/common.h>
#include <solvers/2020/puzzle_2020_12.h>

namespace puzzle_2020_12 {

struct Ship {
  uint distanceToOrigin() {
    return static_cast<uint>(std::abs(m_position.x()) +
                             std::abs(m_position.y()));
  }
  QPoint m_position{0, 0};
  QPoint m_orientation{1, 0};
  QPoint m_waypoint{10, 1};
};

struct NavigationInstruction {
  NavigationInstruction(uint value, bool new_system)
      : m_value{value}, m_new_system{new_system} {}
  virtual ~NavigationInstruction() = default;
  virtual void apply(Ship &) = 0;
  uint m_value{0};
  bool m_new_system{false};
};

struct North : public NavigationInstruction {
  North(uint value, bool new_system)
      : NavigationInstruction{value, new_system} {}
  void apply(Ship &ship) {
    if (m_new_system)
      ship.m_waypoint += static_cast<int>(m_value) * QPoint{0, 1};
    else
      ship.m_position += static_cast<int>(m_value) * QPoint{0, 1};
  }
};

struct South : public NavigationInstruction {
  South(uint value, bool new_system)
      : NavigationInstruction{value, new_system} {}
  void apply(Ship &ship) {
    if (m_new_system)
      ship.m_waypoint += static_cast<int>(m_value) * QPoint{0, -1};
    else
      ship.m_position += static_cast<int>(m_value) * QPoint{0, -1};
  }
};

struct East : public NavigationInstruction {
  East(uint value, bool new_system)
      : NavigationInstruction{value, new_system} {}
  void apply(Ship &ship) {
    if (m_new_system)
      ship.m_waypoint += static_cast<int>(m_value) * QPoint{1, 0};
    else
      ship.m_position += static_cast<int>(m_value) * QPoint{1, 0};
  }
};

struct West : public NavigationInstruction {
  West(uint value, bool new_system)
      : NavigationInstruction{value, new_system} {}
  void apply(Ship &ship) {
    if (m_new_system)
      ship.m_waypoint += static_cast<int>(m_value) * QPoint{-1, 0};
    else
      ship.m_position += static_cast<int>(m_value) * QPoint{-1, 0};
  }
};

struct Left : public NavigationInstruction {
  Left(uint value, bool new_system)
      : NavigationInstruction{value, new_system} {}
  void apply(Ship &ship) {
    uint nb_steps = m_value / 90;
    for (uint i = 0; i < nb_steps; ++i)
      if (m_new_system)
        ship.m_waypoint = QPoint{-ship.m_waypoint.y(), ship.m_waypoint.x()};
      else
        ship.m_orientation =
            QPoint{-ship.m_orientation.y(), ship.m_orientation.x()};
  }
};

struct Right : public NavigationInstruction {
  Right(uint value, bool new_system)
      : NavigationInstruction{value, new_system} {}
  void apply(Ship &ship) {
    uint nb_steps = m_value / 90;
    for (uint i = 0; i < nb_steps; ++i)
      if (m_new_system)
        ship.m_waypoint = QPoint{ship.m_waypoint.y(), -ship.m_waypoint.x()};
      else
        ship.m_orientation =
            QPoint{ship.m_orientation.y(), -ship.m_orientation.x()};
  }
};

struct Forward : public NavigationInstruction {
  Forward(uint value, bool new_system)
      : NavigationInstruction{value, new_system} {}
  void apply(Ship &ship) {
    if (m_new_system)
      ship.m_position += static_cast<int>(m_value) * ship.m_waypoint;
    else
      ship.m_position += static_cast<int>(m_value) * ship.m_orientation;
  }
};

struct Route : public QVector<NavigationInstruction *> {
  Route(const QString &input, bool new_system) {
    bool ok;
    const QStringList lines = common::splitLines(input);
    this->reserve(lines.size());
    for (const QString &line : lines) {
      if (line.size() < 2)
        continue;
      QChar instruction = line.front();
      uint value = line.mid(1).toUInt(&ok);
      if (!ok)
        continue;
      if (instruction == 'N')
        *this << new North(value, new_system);
      else if (instruction == 'S')
        *this << new South(value, new_system);
      else if (instruction == 'E')
        *this << new East(value, new_system);
      else if (instruction == 'W')
        *this << new West(value, new_system);
      else if (instruction == 'L')
        *this << new Left(value, new_system);
      else if (instruction == 'R')
        *this << new Right(value, new_system);
      else if (instruction == 'F')
        *this << new Forward(value, new_system);
    }
  }

  ~Route() {
    for (NavigationInstruction *instruction : *this)
      delete instruction;
  }

  void apply(Ship &ship) {
    for (NavigationInstruction *instruction : *this)
      instruction->apply(ship);
  }
};

} // namespace puzzle_2020_12

void Solver_2020_12_1::solve(const QString &input) {
  using namespace puzzle_2020_12;
  Ship ship;
  Route(input, false).apply(ship);
  emit finished(QString::number(ship.distanceToOrigin()));
}

void Solver_2020_12_2::solve(const QString &input) {
  using namespace puzzle_2020_12;
  Ship ship;
  Route(input, true).apply(ship);
  emit finished(QString::number(ship.distanceToOrigin()));
}
