#include <2018/puzzle_2018_01.h>
#include <common.h>
#include <set>

void Solver_2018_01_1::solve(const QString &input) {
  const auto values = common::toUInt(input, '\n');
  auto sum = 0;
  for (const auto &v : values)
    sum += v;
  emit finished(QString("%1").arg(sum));
}

void Solver_2018_01_2::solve(const QString &input) {
  const auto values = common::toUInt(input, '\n');
  auto seen = std::set<int>{};
  auto frequency = 0;
  for (auto it = std::begin(values); seen.insert(frequency).second; ++it) {
    if (it == std::end(values))
      it = std::begin(values);
    frequency += *it;
  }
  emit finished(QString("%1").arg(frequency));
}
