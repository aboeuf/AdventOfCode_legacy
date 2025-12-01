#include <solvers/2021/puzzle_2021_17.h>
#include <solvers/common.h>

constexpr auto try_area = 500;

struct Target {
  Target(const QString &str) {
    auto rx = QRegExp{"^target area: x=(.+)\\.\\.(.+), y=(.+)\\.\\.(.+).*"};
    m_valid = rx.exactMatch(str);
    if (not m_valid)
      return;
    m_x_min = rx.cap(1).toInt(&m_valid);
    if (not m_valid)
      return;
    m_x_max = rx.cap(2).toInt(&m_valid);
    if (not m_valid)
      return;
    m_y_min = rx.cap(3).toInt(&m_valid);
    if (not m_valid)
      return;
    m_y_max = rx.cap(4).toInt(&m_valid);
    if (not m_valid)
      return;
    if (m_x_max < m_x_min)
      std::swap(m_x_min, m_x_max);
    if (m_y_max < m_y_min)
      std::swap(m_y_min, m_y_max);
  }

  bool m_valid;
  int m_x_min;
  int m_x_max;
  int m_y_min;
  int m_y_max;
};

struct Probe {
  Probe(const Target &target, int vx, int vy)
      : m_x{0}, m_y{0}, m_vx{vx}, m_vy{vy}, m_max_height{0} {
    while (not isIn(target) and not hasMissed(target)) {
      advanceOneStep();
    }
  }

  bool isIn(const Target &target) const {
    return target.m_x_min <= m_x and m_x <= target.m_x_max and
           target.m_y_min <= m_y and m_y <= target.m_y_max;
  }

  bool hasMissed(const Target &target) const {
    return (m_x < target.m_x_min and m_vx <= 0) or
           (m_x > target.m_x_max and m_vx >= 0) or
           (m_y < target.m_y_min and m_vy <= 0);
  }

  int maxHeight() const { return m_max_height; }

private:
  void advanceOneStep() {
    m_x += m_vx;
    m_y += m_vy;
    m_max_height = std::max(m_max_height, m_y);
    if (m_vx != 0)
      m_vx += m_vx < 0 ? 1 : -1;
    m_vy -= 1;
  }

  int m_x;
  int m_y;
  int m_vx;
  int m_vy;
  int m_max_height;
};

class ShotComputer {
public:
  ShotComputer(const QString &target) : m_target{target} {}

  QString solvePuzzleOne() const {
    if (not m_target.m_valid)
      return "Input error";
    auto max_height = 0;
    const auto launch = [&max_height, this](auto vx, auto vy) {
      const auto probe = Probe(m_target, vx, vy);
      if (probe.isIn(m_target))
        max_height = std::max(max_height, probe.maxHeight());
    };
    for (auto vx = -try_area; vx < try_area; ++vx)
      for (auto vy = 0; vy < try_area; ++vy)
        launch(vx, vy);
    return QString("%1").arg(max_height);
  }

  QString solvePuzzleTwo() const {
    if (not m_target.m_valid)
      return "Input error";
    auto nb_traj = 0;
    const auto launch = [&nb_traj, this](auto vx, auto vy) {
      const auto probe = Probe(m_target, vx, vy);
      if (probe.isIn(m_target))
        ++nb_traj;
    };
    for (auto vx = -try_area; vx < try_area; ++vx)
      for (auto vy = -try_area; vy < try_area; ++vy)
        launch(vx, vy);
    return QString("%1").arg(nb_traj);
  }

private:
  Target m_target;
};

void Solver_2021_17_1::solve(const QString &input) {
  emit finished(ShotComputer(input).solvePuzzleOne());
}

void Solver_2021_17_2::solve(const QString &input) {
  emit finished(ShotComputer(input).solvePuzzleTwo());
}
