#include <2019/puzzle_2019_1.h>
#include <common.h>

PuzzleSolver Puzzle_2019_1::solver_1 = [](const QString& input) {
  QStringList lines = common::splitLines(input);
  int sum = 0;
  for (auto i = 0; i < lines.size(); ++i) {
    sum += lines[i].toInt() / 3 - 2;
  }
  return QString::number(sum);
};

PuzzleSolver Puzzle_2019_1::solver_2 = [](const QString& input) {
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
  return QString::number(sum);
};

