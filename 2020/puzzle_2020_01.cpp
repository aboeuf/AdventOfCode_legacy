#include <2020/puzzle_2020_01.h>
#include <common.h>

void Solver_2020_01_1::solve(const QString& input)
{
  QStringList lines = common::splitLines(input);
  for (int i = 0; i < lines.size(); ++i) {
    int input_i = lines[i].toInt();
    for (int j = i + 1; j < lines.size(); ++j) {
      int input_j = lines[j].toInt();
      if (input_i + input_j == 2020) {
        emit finished(QString::number(input_i * input_j));
        return;
      }
    }
  }
  emit finished("FAILURE");
}

void Solver_2020_01_2::solve(const QString& input)
{
  QStringList lines = common::splitLines(input);
  for (int i = 0; i < lines.size(); ++i) {
    int input_i = lines[i].toInt();
    for (int j = i + 1; j < lines.size(); ++j) {
      int input_j = lines[j].toInt();
      for (int k = j + 1; k < lines.size(); ++k) {
        int input_k = lines[k].toInt();
        if (input_i + input_j + input_k == 2020) {
          emit finished(QString::number(input_i * input_j * input_k));
          return;
        }
      }
    }
  }
  emit finished("FAILURE");
}

