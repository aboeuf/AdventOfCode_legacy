#include <2016/puzzle_2016_01.h>
#include <common.h>
#include <QPoint>
#include <set>

struct Walker
{
  Walker(const QString& input, bool stop_at_loop = false)
  {
    if (stop_at_loop)
      m_history.insert(m_position);
    auto instructions = common::splitValues(input);
    for (auto& instruction : instructions) {
      if (move(instruction, stop_at_loop))
        return;
    }
  }

  bool move(QString& instruction, bool stop_at_loop)
  {
    instruction.remove(' ');
    auto rx = QRegExp{"^(R|L)(\\d+)$"};
    if (rx.exactMatch(instruction)) {
      m_direction = QPoint(-m_direction.y(), m_direction.x());
      if (rx.cap(1) == "R")
        m_direction = -m_direction;
      if (stop_at_loop) {
        for (auto i = 0; i < rx.cap(2).toInt(); ++i) {
          m_position += m_direction;
          if (not m_history.insert(m_position).second)
            return true;
        }
      } else
        m_position += rx.cap(2).toInt() * m_direction;
    }
    return false;
  }

  int dist() const { return m_position.manhattanLength(); }

  QPoint m_position{0, 0};
  QPoint m_direction{0, 1};
  using Comp = std::function<bool(const QPoint&, const QPoint&)>;
  std::set<QPoint, Comp> m_history{[](const QPoint& lhs, const QPoint& rhs) {
      if (lhs.x() == rhs.x())
        return lhs.y() < rhs.y();
      return lhs.x() < rhs.x();
    }};
};

void Solver_2016_01_1::solve(const QString& input)
{
  emit finished(QString("%1").arg(Walker(input).dist()));
}

void Solver_2016_01_2::solve(const QString& input)
{
  emit finished(QString("%1").arg(Walker(input, true).dist()));
}
