#include <2019/puzzle_2019_1.h>
#include <common.h>

void Solver_2019_1_1::solve(const QString& input)
{
  QStringList lines = common::splitLines(input);
  int sum = 0;
  for (auto i = 0; i < lines.size(); ++i) {
    sum += lines[i].toInt() / 3 - 2;
  }
  emit finished(QString::number(sum));
}

void Solver_2019_1_2::solve(const QString& input)
{
  QStringList lines = common::splitLines(input);
  int sum = 0;
  for (auto i = 0; i < lines.size(); ++i) {
    int mass = lines[i].toInt();
    do
    {
      mass = std::max(mass / 3 - 2, 0);
      sum += mass;
    } while (mass > 0);
  }
  emit finished(QString::number(sum));
}

