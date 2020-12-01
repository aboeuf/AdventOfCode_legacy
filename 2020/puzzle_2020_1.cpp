#include <2020/puzzle_2020_1.h>
#include <common.h>

Solvers::Solver Puzzle_2020_1::solver = [](const QString& input)
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

