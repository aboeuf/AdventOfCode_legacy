#include <solvers/common.h>
#include <solvers/2021/puzzle_2021_06.h>

using Int = unsigned long long int;

class Fishes {
public:
  Fishes(const QString &input) {
    const auto values = common::toVecUInt(input);
    for (const auto &v : values) {
      if (v < 9u)
        ++m_nb_per_age[v];
    }
  }

  Int nbFishes() const {
    auto nb_fishes = Int{0};
    for (auto i = 0; i < 9; ++i)
      nb_fishes += m_nb_per_age[i];
    return nb_fishes;
  }

  void increment() {
    const auto nb_new = m_nb_per_age[0];
    for (auto i = 0; i < 8; ++i)
      m_nb_per_age[i] = m_nb_per_age[i + 1];
    m_nb_per_age[8] = nb_new;
    m_nb_per_age[6] += nb_new;
  }

  QString solvePuzzleOne() {
    for (auto i = 0; i < 80; ++i)
      increment();
    return QString{"%1"}.arg(nbFishes());
  }

  QString solvePuzzleTwo() {
    for (auto i = 0; i < 256; ++i)
      increment();
    return QString{"%1"}.arg(nbFishes());
  }

private:
  std::array<Int, 9> m_nb_per_age{0, 0, 0, 0, 0, 0, 0, 0, 0};
};

void Solver_2021_06_1::solve(const QString &input) {
  auto fishes = Fishes{input};
  emit finished(fishes.solvePuzzleOne());
}

void Solver_2021_06_2::solve(const QString &input) {
  auto fishes = Fishes{input};
  emit finished(fishes.solvePuzzleTwo());
}
