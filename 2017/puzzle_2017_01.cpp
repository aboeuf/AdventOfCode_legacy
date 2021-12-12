#include <2017/puzzle_2017_01.h>
#include <common.h>

void Solver_2017_01_1::solve(const QString& input)
{
  auto copy = input;
  copy.remove('\n');
  auto sum = 0;
  for (int i = 0; i < copy.size(); ++i) {
    auto next = (i + 1) % copy.size();
    if (copy[i] == copy[next]) {
      sum += QString(copy[i]).toInt();
    }
  }
  emit finished(QString("%1").arg(sum));
}

void Solver_2017_01_2::solve(const QString& input)
{
  auto copy = input;
  copy.remove('\n');
  auto sum = 0;
  for (int i = 0; i < copy.size(); ++i) {
    auto next = (i + (copy.size() / 2)) % copy.size();
    if (copy[i] == copy[next]) {
      sum += QString(copy[i]).toInt();
    }
  }
  emit finished(QString("%1").arg(sum));
}
