#include <2020/puzzle_2020_1.h>
#include <common.h>

PuzzleSolver Puzzle_2020_1::solver_1 = [](const QString& input)
{
  QStringList lines = common::splitLines(input);
  for (int i = 0; i < lines.size(); ++i) {
    int input_i = lines[i].toInt();
    for (int j = i + 1; j < lines.size(); ++j) {
      int input_j = lines[j].toInt();
      if (input_i + input_j == 2020)
        return QString::number(input_i * input_j);
    }
  }
  return QString{"FAILURE"};
};

PuzzleSolver Puzzle_2020_1::solver_2 = [](const QString& input)
{
  QStringList lines = common::splitLines(input);
  for (int i = 0; i < lines.size(); ++i) {
    int input_i = lines[i].toInt();
    for (int j = i + 1; j < lines.size(); ++j) {
      int input_j = lines[j].toInt();
      for (int k = j + 1; k < lines.size(); ++k) {
        int input_k = lines[k].toInt();
        if (input_i + input_j + input_k == 2020)
          return QString::number(input_i * input_j * input_k);
      }
    }
  }
  return QString{"FAILURE"};
};

