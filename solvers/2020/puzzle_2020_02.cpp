#include <solvers/2020/puzzle_2020_02.h>
#include <solvers/common.h>

namespace puzzle_2020_02 {

bool extractParameters(const QString &input, int &i0, int &i1, QChar &c,
                       QString &password) {
  QStringList list = input.split(' ');
  if (list.size() < 3)
    return false;
  if (list[0].isEmpty() || list[1].isEmpty())
    return false;
  QStringList int_values = list[0].split('-');
  if (int_values.size() < 2 || int_values[0].isEmpty() ||
      int_values[1].isEmpty())
    return false;
  bool ok;
  i0 = int_values[0].toInt(&ok);
  if (!ok)
    return false;
  i1 = int_values[1].toInt(&ok);
  if (!ok)
    return false;
  c = list[1].front();
  password = list[2];
  return true;
}

bool isValidSlendRental(const QString &input) {
  int min, max;
  QChar c;
  QString password;
  if (!extractParameters(input, min, max, c, password))
    return false;
  int count = password.count(c);
  return min <= count && count <= max;
}

bool isValidToboganCorporate(const QString &input) {
  int i0, i1;
  QChar c;
  QString password;
  if (!extractParameters(input, i0, i1, c, password))
    return false;
  --i0;
  --i1;
  if (i0 < 0 || i1 < 0 || i0 >= password.size() || i1 >= password.size())
    return false;
  return (password[i0] == c) ^ (password[i1] == c);
}

} // namespace puzzle_2020_02

void Solver_2020_02_1::solve(const QString &input) {
  QStringList lines = common::splitLines(input);
  unsigned int nb_valids = 0u;
  for (const QString &line : lines)
    if (puzzle_2020_02::isValidSlendRental(line))
      ++nb_valids;
  emit finished(QString::number(nb_valids));
}

void Solver_2020_02_2::solve(const QString &input) {
  QStringList lines = common::splitLines(input);
  unsigned int nb_valids = 0u;
  for (const QString &line : lines)
    if (puzzle_2020_02::isValidToboganCorporate(line))
      ++nb_valids;
  emit finished(QString::number(nb_valids));
}
