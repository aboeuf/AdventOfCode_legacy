#include <2020/puzzle_2020_5.h>
#include <common.h>
#include <set>

namespace puzzle_2020_5
{

int pow2(int n)
{
  if (n <= 0)
    return 1;
  return 2 * pow2(n - 1);
}

int ID(int row, int column) {
  return 8 * row + column;
}

bool binaryFind(const QString& boarding_pass,
                int start, int count,
                const QChar& down, const QChar& up,
                int& result)
{
  int min = 0;
  int max = pow2(count) - 1;
  result = (max + min) / 2;
  for (int i = start; i < start + count; ++i)
  {
    if (i >= boarding_pass.size())
      return false;
    if (boarding_pass[i] == down)
      max = result;
    else if (boarding_pass[i] == up)
      min = result + 1;
    else
      return false;
    result = (max + min) / 2;
  }
  return true;
}

bool decodeBoardingPass(const QString& boarding_pass, int& row, int& column, int& id)
{
  if (boarding_pass.size() != 10)
    return false;
  if (!binaryFind(boarding_pass, 0, 7, 'F', 'B', row))
    return false;
  if (!binaryFind(boarding_pass, 7, 3, 'L', 'R', column))
    return false;
  id = ID(row, column);
  return true;
}
}

void Solver_2020_5_1::solve(const QString& input) const
{
  using namespace puzzle_2020_5;
  const QStringList boarding_passes = common::splitLines(input);
  int row, column, id, max_id = -1;
  for (const QString& boarding_pass : boarding_passes) {
    if (decodeBoardingPass(boarding_pass, row, column, id) && id > max_id)
      max_id = id;
  }
  emit finished(QString::number(max_id));
}

void Solver_2020_5_2::solve(const QString& input) const
{
  using namespace puzzle_2020_5;
  constexpr int nb_rows = 128;
  constexpr int nb_columns = 8;
  std::vector<std::vector<bool>> seats;
  seats.resize(nb_rows);
  std::set<int> ids;
  for (int i = 0; i < nb_rows; ++i) {
    seats[i].resize(nb_columns, false);
  }
  const QStringList boarding_passes = common::splitLines(input);
  int row, column, id;
  for (const QString& boarding_pass : boarding_passes) {
    if (decodeBoardingPass(boarding_pass, row, column, id)) {
      seats[row][column] = true;
      ids.insert(id);
    }
  }
  for (int row = 0; row< nb_rows; ++row) {
    for (int column = 0; column < nb_columns; ++column) {
      if (!seats[row][column]) {
        int id = ID(row, column);
        if (ids.find(id - 1) != ids.end() && ids.find(id + 1) != ids.end()) {
          emit finished(QString::number(id));
          return;
        }
      }
    }
  }
  emit finished("FAILURE");
}

