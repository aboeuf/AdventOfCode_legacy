#include <solvers/common.h>
#include <solvers/2015/puzzle_2015_01.h>

void Solver_2015_01_1::solve(const QString &input) {
  auto floor = 0;
  for (const auto &c : input)
    floor += c == '(' ? 1 : -1;
  emit finished(QString("%1").arg(floor));
}

void Solver_2015_01_2::solve(const QString &input) {
  auto floor = 0;
  for (auto i = 0; i < input.size(); ++i) {
    floor += input[i] == '(' ? 1 : -1;
    if (floor < 0) {
      emit finished(QString("%1").arg(i + 1));
      return;
    }
  }
  emit finished("error");
}
