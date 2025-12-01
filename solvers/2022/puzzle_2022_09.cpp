#include <solvers/common.h>
#include <functional>
#include <set>
#include <solvers/2022/puzzle_2022_09.h>

namespace puzzle_2022_09 {

struct Knot {
  int x{0};
  int y{0};

  bool operator<(const Knot &other) const {
    if (x == other.x)
      return y < other.y;
    return x < other.x;
  }
};

class Rope {
public:
  Rope(std::size_t nb_m_knots) {
    const auto n = std::max(std::size_t{2}, nb_m_knots);
    m_knots.resize(n, Knot{});
  }

  void move(const QString &direction) {
    if (direction == "R")
      ++m_knots[0].x;
    else if (direction == "L")
      --m_knots[0].x;
    else if (direction == "U")
      ++m_knots[0].y;
    else if (direction == "D")
      --m_knots[0].y;
    else
      throw std::invalid_argument(QString("unknown direction \"%1\" to uint")
                                      .arg(direction)
                                      .toStdString());
    for (auto i = 1u; i < m_knots.size(); ++i) {
      const auto dx = m_knots[i - 1].x - m_knots[i].x;
      const auto move_x = std::abs(dx) > 1;
      if (move_x) {
        if (dx > 0)
          ++m_knots[i].x;
        else
          --m_knots[i].x;
      }
      const auto dy = m_knots[i - 1].y - m_knots[i].y;
      const auto move_y = std::abs(dy) > 1;
      if (move_y) {
        if (dy > 0)
          ++m_knots[i].y;
        else
          --m_knots[i].y;
      }
      if (move_x and not move_y)
        m_knots[i].y = m_knots[i - 1].y;
      else if (move_y and not move_x)
        m_knots[i].x = m_knots[i - 1].x;
    }
  }

  const Knot &tail() const { return m_knots.back(); }

private:
  std::vector<Knot> m_knots;
};

uint getNbVistedPositons(const QString &input, std::size_t nb_knots) {
  // Init
  bool ok;
  auto rope = Rope{nb_knots};
  auto visited_positions = std::set<Knot>{};
  const auto log_position = [&rope, &visited_positions]() {
    visited_positions.insert(rope.tail());
  };
  log_position();

  // Move rope
  const auto lines = common::splitLines(input);
  for (const auto &line : lines) {
    const auto values = common::splitValues(line, ' ');
    const auto nb_steps = values[1].toUInt(&ok);
    if (not ok)
      throw std::invalid_argument(QString("cannot convert \"%1\" to uint")
                                      .arg(values[1])
                                      .toStdString());
    for (auto i = 0u; i < nb_steps; ++i) {
      rope.move(values[0]);
      log_position();
    }
  }

  return visited_positions.size();
}

} // namespace puzzle_2022_09

void Solver_2022_09_1::solve(const QString &input) {
  const auto nb_visited = puzzle_2022_09::getNbVistedPositons(input, 2);
  emit finished(QString("%1").arg(nb_visited));
}

void Solver_2022_09_2::solve(const QString &input) {
  const auto nb_visited = puzzle_2022_09::getNbVistedPositons(input, 10);
  emit finished(QString("%1").arg(nb_visited));
}
